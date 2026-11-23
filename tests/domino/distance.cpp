// Supplemental analytic geometry checks. These do not establish binary matches.
#include "Contrib/Contrib/Domino/Collision/Functions/dmDistance.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

bool dm_assertOnce = false;
void dmAssertFunction(const char * message, const char * file, dm_int32 line, void *, void *)
{
    std::fprintf(stderr, "%s:%d: %s\n", file, line, message);
    std::abort();
}

static dmTransform identity()
{
    dmTransform xf;
    xf.q = dmVecSet(0.f, 0.f, 0.f, 1.f);
    xf.p = dmVecSet(0.f);
    return xf;
}

static void distance(const dm_vec4 * a, int na, const dm_vec4 * b, int nb,
                     float expected, float ra = 0.f, float rb = 0.f)
{
    dmDistanceInput input;
    input.proxyA.Set(a, na, dmVecSet(ra));
    input.proxyB.Set(b, nb, dmVecSet(rb));
    input.transformA = identity();
    input.transformB = identity();
    input.cache.Clear();
    input.useRadii = ra != 0.f || rb != 0.f;
    for (int pass = 0; pass != 2; ++pass)
    {
        dmDistanceOutput output;
        dmDistance(&output, &input);
        float actual = dmF32(output.distance);
        if (!(std::isfinite(actual) && std::fabs(actual - expected) < 0.001f))
        {
            std::fprintf(stderr, "distance %g != %g (cache pass %d)\n", actual, expected, pass);
            std::abort();
        }
        input.cache = output.cache;
    }
}

static void supportCases()
{
    // Distinct fourth lanes verify that selection returns the entire vertex,
    // while the support dot product considers only its first three lanes.
    const dm_vec4 vertices[] = {
        dmVecSet(-8.f, -3.f, -2.f, 10.f),
        dmVecSet(-4.f, -6.f, -1.f, 20.f),
        dmVecSet(-2.f, -7.f, -5.f, 30.f),
        dmVecSet(-2.f, -7.f, -5.f, 40.f),
        dmVecSet(-9.f, -8.f, -9.f, 50.f)};
    const dm_vec4 directions[] = {
        dmVecSet(1.f, 0.f, 0.f), dmVecSet(-1.f, 0.f, 0.f),
        dmVecSet(0.f, 1.f, 0.f), dmVecSet(0.f, -1.f, 0.f),
        dmVecSet(0.f, 0.f, 1.f), dmVecSet(0.f, 0.f, -1.f), dmVecSet(0.f)};
    const int expected[] = {2, 4, 0, 4, 1, 4, 0};
    dmGJKProxy proxy;
    proxy.Set(vertices, 5, dmVecSet(0.f));
    dm_vec4 result;
    for (int i = 0; i != 7; ++i)
    {
        assert(proxy.GetSupport(&result, directions[i]) == expected[i]);
        assert(std::memcmp(&result, &vertices[expected[i]], sizeof(result)) == 0);
    }
    // Unordered comparisons retain the first support, as do ordinary ties.
    assert(proxy.GetSupport(&result, dmVecSet(std::numeric_limits<float>::quiet_NaN())) == 0);
    assert(std::memcmp(&result, &vertices[0], sizeof(result)) == 0);
    proxy.Set(vertices, 1, dmVecSet(0.f));
    assert(proxy.GetSupport(&result, directions[0]) == 0);
    assert(std::memcmp(&result, &vertices[0], sizeof(result)) == 0);
}

int main()
{
    supportCases();
    const dm_vec4 point[] = {dmVecSet(0.f, 0.f, 0.f)};
    const dm_vec4 other[] = {dmVecSet(3.f, 4.f, 0.f)};
    distance(point, 1, other, 1, 5.f);
    distance(point, 1, other, 1, 3.f, 1.f, 1.f);
    distance(point, 1, point, 1, 0.f);
    const dm_vec4 segment[] = {dmVecSet(-1.f, 0.f, 0.f), dmVecSet(1.f, 0.f, 0.f)};
    const dm_vec4 above[] = {dmVecSet(0.f, 2.f, 0.f)};
    distance(segment, 2, above, 1, 2.f);
    dmGJKProxy proxy;
    proxy.Set(segment, 2, dmVecSet(0.f));
    dm_vec4 support;
    assert(proxy.GetSupport(&support, dmVecSet(0.f, 1.f, 0.f)) == 0);
    assert(proxy.GetSupport(&support, dmVecSet(1.f, 0.f, 0.f)) == 1);
    dm_vec4 cube[8];
    for (int i = 0; i != 8; ++i)
        cube[i] = dmVecSet(i & 1 ? 1.f : -1.f, i & 2 ? 1.f : -1.f, i & 4 ? 1.f : -1.f);
    const dm_vec4 outside[] = {dmVecSet(3.f, 0.f, 0.f)};
    distance(cube, 8, outside, 1, 2.f);
    distance(cube, 8, point, 1, 0.f);
    // Sweep a deterministic grid around and through the box: its analytic
    // point distance exercises face, edge, vertex and interior simplices.
    for (int x = -3; x <= 3; ++x)
        for (int y = -3; y <= 3; ++y)
            for (int z = -3; z <= 3; ++z)
            {
                const dm_vec4 sample[] = {dmVecSet(float(x), float(y), float(z))};
                float dx = std::fmax(std::fabs(float(x)) - 1.f, 0.f);
                float dy = std::fmax(std::fabs(float(y)) - 1.f, 0.f);
                float dz = std::fmax(std::fabs(float(z)) - 1.f, 0.f);
                distance(cube, 8, sample, 1, std::sqrt(dx * dx + dy * dy + dz * dz));
            }
    const dm_vec4 tetra[] = {dmVecSet(1.f, 0.f, 0.f), dmVecSet(0.f, 1.f, 0.f),
                            dmVecSet(0.f, 0.f, 1.f), dmVecSet(-1.f, -1.f, -1.f)};
    distance(tetra, 4, point, 1, 0.f);
    const dm_vec4 triangle[] = {dmVecSet(-1.f, -1.f, 0.f), dmVecSet(1.f, -1.f, 0.f), dmVecSet(0.f, 1.f, 0.f)};
    const dm_vec4 normal[] = {dmVecSet(0.f, 0.f, 3.f)};
    distance(triangle, 3, normal, 1, 3.f);
    std::puts("Domino analytic distance/cache/support checks passed");
}
