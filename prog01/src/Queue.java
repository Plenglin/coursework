public class Queue<T> {
    private DoublyLinkedList<T> list = new DoublyLinkedList<>();

    public void enqueue(T obj) {
        list.insertBack(obj);
    }

    public T dequeue() {
        if (list.size == 0) {
            return null;
        }
        T out = list.findFromHead(0);
        list.removeFront();
        return out;
    }

    public T getFirst() {
        if (list.size == 0) {
            return null;
        }
        return list.findFromHead(0);
    }

    public T getLast() {
        if (list.size == 0) {
            return null;
        }
        return list.findFromTail(0);
    }
}
