// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdSort.inl
// Bodies are not in the DWARF; written from the one instantiation (bdReference<MailInternal>,
// 0x71000476c0). Lines in the comments are the original ones.

// line 11
template <typename T, typename Compare>
void bdCombSort(T *const data, const bdUInt size, Compare compare)
{
    if (size > 1)  // line 15
    {
        bdUInt gap = size;
        bdBool swapped;
        do
        {
            swapped = false;
            bdNewGap(gap);  // line 20
            for (bdUInt i = 0; i < size - gap; ++i)  // line 22
            {
                bdUInt j = i + gap;
                T & one = data[i];
                T & two = data[j];
                if (compare(two, one))  // line 28
                {
                    T temp(one);
                    one = two;
                    two = temp;
                    swapped = true;
                }
            }
        } while (gap != 1 || swapped);  // line 36
    }
}

// line 52
inline void bdNewGap(bdUInt & gap)
{
    gap = (gap * 10) / 13;
    if (gap == 9 || gap == 10)
    {
        gap = 11;
    }
    if (gap < 1)
    {
        gap = 1;
    }
}
