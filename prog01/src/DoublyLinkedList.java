public class DoublyLinkedList<T> {

    protected final Node<T> head;
    protected int size;

    public DoublyLinkedList() {
        head = new Node<>(null);
        head.next = head;
        head.prev = head;
        size = 0;
    }

    public DoublyLinkedList(T a) {
        this();
        insertBack(a);
    }

    public DoublyLinkedList(T a, T b) {
        this();
        insertBack(a);
        insertBack(b);
    }

    public void insertFront(T obj) {
        Node<T> node = new Node<>(obj);
        Node<T> head = this.head.next;
        node.prev = this.head;
        node.next = head;
        head.prev = node;
        this.head.next = node;
        size++;
    }

    public void insertBack(T obj) {
        Node<T> node = new Node<>(obj);
        Node<T> tail = head.prev;
        node.next = head;
        node.prev = tail;
        tail.next = node;
        head.prev = node;
        size++;
    }

    public void removeFront() {
        if (size > 0) {
            Node<T> newHead = head.next.next;
            newHead.prev = head;
            head.next = newHead;
            size--;
        }
    }

    public void removeTail() {
        if (size > 0) {
            Node<T> newTail = head.prev.prev;
            newTail.next = head;
            head.prev = newTail;
            size--;
        }
    }

    public T findFromHead(int i) {
        if (0 > i || i >= size) {
            throw new IndexOutOfBoundsException(String.format("%s is out of bounds! (size = %s)", i, size));
        }
        Node<T> node = head.next;
        while (i > 0 && node != head) {
            node = node.next;
            i--;
        }
        return node.item;
    }

    public T findFromTail(int i) {
        if (0 > i || i >= size) {
            throw new IndexOutOfBoundsException(String.format("%s is out of bounds! (size = %s)", i, size));
        }
        Node<T> node = head.prev;
        while (i > 0 && node != head) {
            node = node.prev;
            i--;
        }
        return node.item;
    }

    @Override
    public String toString() {
        if (size == 0) {
            return "[]";
        }
        StringBuilder str = new StringBuilder("[");
        Node node = head.next;
        while (node != head.prev) {
            str.append(String.valueOf(node.item));
            str.append(", ");
            node = node.next;
        }
        str.append(head.prev.item.toString());
        str.append("]");
        return str.toString();
    }

}
