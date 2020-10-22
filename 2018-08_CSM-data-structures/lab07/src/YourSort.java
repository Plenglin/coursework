/* YourSort.java */

import java.util.Arrays;

public class YourSort {

    public static void sort(int[] arr) {
        // Place your Part III code here.
        sort(arr, 0, arr.length - 1);

        /*
        plot "insert.dat" t "INSERT" w l lt 1 lw 3\
        , "merge.dat" t "MERGE"  w l lt 2 lw 3\
        , "quick.dat" t "QUICK"  w l lt 3 lw 3\
        , "best.dat" t "BEST"  w l lt 4 lw 3
         */
    }

    private static void sort(int[] arr, int start, int end) {
        if (end - start > 20) {
            // Perform quicksort
            int lo = start;
            int hi = end;
            int mid;

            if (end > start) {
                swap(arr, start, (start + end)/2);
                mid = arr[(start + end) / 2];
                while (lo <= hi) {
                    while((lo < end) && (arr[lo] < mid)) {
                        lo++;
                    }
                    while((hi > start) && (arr[hi] > mid)) {
                        hi--;
                    }
                    if (lo <= hi) {
                        swap(arr, lo, hi);
                        lo++;
                        hi--;
                    }
                }
                if (start < hi) {
                    sort(arr, start, hi);
                }

                if (lo < end) {
                    sort(arr, lo, end);
                }
            }
        } else {
            // Perform insertion sort
            insertionSort(arr, start, end);
        }
    }

    private static void swap(int[] arr, int a, int b) {
        int tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = tmp;
    }

    private static void insertionSort(int[] arr, int start, int end) {
        for (int i=start; i <= end; i++) {
            int x = arr[i];
            int j = i;
            while (start < j && x < arr[j - 1]) {
                arr[j] = arr[j - 1];
                j--;
            }
            arr[j] = x;
        }
    }

    public static void main(String[] args) {
        int[] test = {
                308,
                6730,
                8318,
                24858,
                32068,
                7882,
                29159,
                31859,
                191,
                11204,
                22408,
                22813,
                14025,
                28778,
                22232,
                6429,
                25111,
                4469,
                8570,
                20680,
                4166,
                32341,
                14331,
                32218,
                2019,
                2244,
                7969,
                20955,
                12943,
                29331,
                11779,
                28107,
                20532,
                12119,
                31151,
                2105,
                29821,
                28263,
                21812,
                2371,
                7128,
                10856,
                13138,
                24294,
                23292,
                32329,
                31076,
                20820,
                2887,
                32388,
                11038,
                20868,
                28078,
                30762,
                15796,
                5022,
                4951,
                29696,
                20474,
                23105,
                29693,
                29413,
                20665,
                12045,
                13108,
                7504,
                8676,
                4052,
                18830,
                11173,
                24525,
                16422,
                11200,
                204,
                25628,
                29280,
                15419,
                27018,
                32141,
                15420,
                13472,
                38,
                19684,
                5106,
                31822,
                971,
                7992,
                7844,
                21425,
                11332,
                15961,
                31122,
                28195,
                24115,
                204,
                31108,
                23743,
                22,
                23566,
                22412
        };
        //int[] test = {2, 1, 4, 9, 5, 8, 3, 7};
        sort(test);
        System.out.println(Arrays.toString(test));
    }

}