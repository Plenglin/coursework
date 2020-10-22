public class DisjointSets {

    public static final int ROOT = -1;
    private int[] data;

    public DisjointSets(int size) {
        data = new int[size];
        for (int i=0; i < size; i++) {
            data[i] = ROOT;
        }
    }

    public void union(int a, int b) {
        data[find(a)] = b;
    }

    public int find(int a) {
        int parent = data[a];
        if (parent == ROOT) {
            return a;
        }
        int root = find(data[a]);
        data[a] = root;
        return root;
    }

}
