// Supplemental analytic sweep/TOI checks; these do not establish binary matches.
#include "Contrib/Contrib/Domino/Collision/Functions/dmTimeOfImpact.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

bool dm_assertOnce = false;
void dmAssertFunction(const char * message, const char * file, dm_int32 line, void *, void *)
{
    std::fprintf(stderr, "%s:%d: %s\n", file, line, message);
    std::abort();
}
static void near(float actual, float expected, float tolerance = 0.0001f)
{
    if (!std::isfinite(actual) || std::fabs(actual - expected) > tolerance)
    {
        std::fprintf(stderr, "%g != %g (tolerance %g)\n", actual, expected, tolerance);
        std::abort();
    }
}
static dmSweep sweep(float start, float end)
{
    dmSweep result;
    result.m_center0 = dmVecSet(start, 0.f, 0.f);
    result.m_center = dmVecSet(end, 0.f, 0.f);
    result.m_quaternion0 = result.m_quaternion = dmVecSet(0.f, 0.f, 0.f, 1.f);
    result.m_localCenter = dmVecSet(0.f);
    result.m_alpha0 = dmVecSet(0.f);
    return result;
}
static void impact(float start, float end, dmTOICode expected, float eta)
{
    const dm_vec4 point[] = { dmVecSet(0.f, 0.f, 0.f) };
    dmTOIInput input;
    input.cache.Clear();
    input.proxyA.Set(point, 1, dmVecSet(0.f));
    input.proxyB.Set(point, 1, dmVecSet(0.f));
    input.sweepA = sweep(0.f, 0.f);
    input.sweepB = sweep(start, end);
    dmTOIOutput output;
    dmTimeOfImpact(&output, &input);
    if (output.exitCode != expected)
    {
        std::fprintf(stderr, "TOI exit %d != %d\n", output.exitCode, expected);
        std::abort();
    }
    near(dmF32(output.eta), eta, 0.001f);
}
int main()
{
    dmSweep motion = sweep(2.f, 10.f);
    motion.m_localCenter = dmVecSet(1.f, 0.f, 0.f);
    near(dmF32(motion.GetTransform(dmVecSet(0.f)).p), 1.f);
    near(dmF32(motion.GetTransform(dmVecSet(1.f)).p), 9.f);
    near(dmF32(motion.GetTransform(dmVecSet(0.5f)).p), 5.f);
    motion.Advance(dmVecSet(0.25f));
    near(dmF32(motion.m_center0), 4.f);
    near(dmF32(motion.m_alpha0), 0.25f);
    near(dmF32(motion.GetTransform(dmVecSet(0.f)).p), 3.f);
    near(dmF32(motion.GetTransform(dmVecSet(1.f)).p), 9.f);
    motion.Advance(dmVecSet(0.25f));
    motion.Advance(dmVecSet(0.1f));
    near(dmF32(motion.m_center0), 4.f);
    motion.Advance(dmVecSet(0.5f));
    near(dmF32(motion.m_center0), 6.f);
    motion.Advance(dmVecSet(1.f));
    near(dmF32(motion.m_center0), 10.f);
    motion.Advance(dmVecSet(1.f));
    near(dmF32(motion.m_alpha0), 1.f);
    // A quarter-turn quaternion and its antipode must interpolate identically.
    motion = sweep(0.f, 0.f);
    motion.m_quaternion0 = dmVecSet(0.f, 0.f, 0.707106781f, 0.707106781f);
    motion.m_quaternion = dmNegate(motion.m_quaternion0);
    dm_vec4 rotated = dmRotate(motion.GetTransform(dmVecSet(0.5f)).q, dmVecSet(1.f, 0.f, 0.f));
    near(dmF32(rotated), 0.f);
    near(vgetq_lane_f32(rotated, 1), 1.f);
    // Exercise every normalization lane and non-unit quaternion interpolation.
    // The scalar oracle retains NLerp's shortest-arc sign decision.
    const float first[4] = {0.3f, -0.7f, 0.2f, 1.5f};
    const float last[4] = {-0.4f, 0.1f, 1.2f, 0.8f};
    for (int sign = -1; sign <= 1; sign += 2)
    {
        for (int step = 0; step <= 10; ++step)
        {
            float eta = step / 10.f;
            motion = sweep(0.f, 0.f);
            motion.m_quaternion0 = vld1q_f32(first);
            motion.m_quaternion = vmulq_n_f32(vld1q_f32(last), float(sign));
            dm_vec4 q = motion.GetTransform(dmVecSet(eta)).q;
            float actual[4];
            vst1q_f32(actual, q);
            double expected[4], lengthSquared = 0;
            for (int lane = 0; lane < 4; ++lane)
            {
                expected[lane] = first[lane] + double(eta) * (last[lane] - first[lane]);
                lengthSquared += expected[lane] * expected[lane];
            }
            for (int lane = 0; lane < 4; ++lane)
                near(actual[lane], float(expected[lane] / std::sqrt(lengthSquared)));
            near(dmF32(dmDot4(q, q)), 1.f);
            if (step != 0)
            {
                motion.Advance(dmVecSet(eta));
                vst1q_f32(actual, motion.m_quaternion0);
                for (int lane = 0; lane < 4; ++lane)
                    near(actual[lane], float(expected[lane] / std::sqrt(lengthSquared)));
            }
        }
    }
    impact(2.f, 2.f, dm_toiMiss, 1.f);
    // Zero-radius point contacts use the recovered positive world-space target.
    impact(2.f, -2.f, dm_toiHit, (2.f - 0.0164041984f) / 4.f);
    impact(0.f, 0.f, dm_toiOverlap, 0.f);
    std::puts("Domino analytic sweep/time-of-impact checks passed");
}
