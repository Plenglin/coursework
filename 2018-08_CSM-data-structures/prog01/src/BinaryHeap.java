import java.util.Arrays;

public class BinaryHeap<T> {

    private int[] keys;
    private Object[] values;
    private int size = 0;
    private int maxSize;

    public BinaryHeap(int capacityFactor) {
        maxSize = 1 << capacityFactor;
        keys = new int[maxSize];
        values = new Object[maxSize];
    }

    public void insert(int key, Object value) {
        size++;
        keys[size] = key;
        values[size] = value;
        bubbleUp(size);
    }

    public IntKeyPair<T> removeMin() {
        if (size == 0) {
            return null;
        }
        IntKeyPair<T> out = new IntKeyPair<>(keys[1], (T) values[1]);
        swap(1, size);
        size--;
        bubbleDown(1);
        return out;
    }

    private void swap(int i1, int i2) {
        int tmpKey = keys[i1];
        Object tmpVal = values[i1];
        keys[i1] = keys[i2];
        values[i1] = values[i2];
        keys[i2] = tmpKey;
        values[i2] = tmpVal;
    }

    private void bubbleUp(int index) {
        int dest = index / 2;
        if (dest == 0) {
            return;
        }
        if (keys[index] < keys[dest]) {
            swap(index, dest);
            bubbleUp(dest);
        }
    }

    private void bubbleDown(int index) {
        int dest1 = index * 2;
        int dest2 = dest1 + 1;
        if (dest1 <= size && keys[dest1] < keys[index]) {
            swap(index, dest1);
            bubbleDown(dest2);
        } else if (dest2 <= size && keys[dest2] < keys[index]) {
            swap(index, dest2);
            bubbleDown(dest2);
        }
    }

    public int getSize() {
        return size;
    }

    @Override
    public String toString() {
        StringBuilder data = new StringBuilder();
        data.append("[");
        data.append(keys[1]);
        data.append(":");
        data.append(values[1]);
        for (int i=2; i <= size; i++) {
            data.append(", ");
            data.append(keys[i]);
            data.append(":");
            data.append(values[i]);
        }
        data.append("]");
        return String.format("Heap(size=%s, data=%s)", size, data);
    }

    public static void main(String[] args) {
        BinaryHeap<String> heap = new BinaryHeap<>(4);

        System.out.println("0");
        heap.insert(5, "A");
        System.out.println(heap.toString());
        heap.insert(4, "B");
        System.out.println(heap.toString());
        heap.insert(7, "F");
        System.out.println(heap.toString());
        heap.insert(1, "D");
        System.out.println(heap.toString());

        System.out.println("4.");
        heap.removeMin( );
        System.out.println(heap.toString());
        heap.insert(3, "J");
        System.out.println(heap.toString());
        heap.insert(6, "L");
        System.out.println(heap.toString());
        heap.removeMin( );
        System.out.println(heap.toString());

        System.out.println("8.");
        heap.removeMin( );
        System.out.println(heap.toString());
        heap.insert(8, "G");
        System.out.println(heap.toString());
        heap.removeMin( );
        System.out.println(heap.toString());
        heap.insert(2, "H");
        System.out.println(heap.toString());

        System.out.println("12.");
        heap.removeMin( );
        System.out.println(heap.toString());
        heap.removeMin( );
        System.out.println(heap.toString());
    }

}
