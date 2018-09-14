public class DoublyLinkedList<T> {

    protected final DoublyLinkedListNode<T> head;
    protected int size;

    public DoublyLinkedList() {
        head = new DoublyLinkedListNode<>(null);
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
        DoublyLinkedListNode<T> node = new DoublyLinkedListNode<>(obj);
        DoublyLinkedListNode<T> head = this.head.next;
        node.prev = this.head;
        node.next = head;
        head.prev = node;
        this.head.next = node;
        size++;
    }

    public void insertBack(T obj) {
        DoublyLinkedListNode<T> node = new DoublyLinkedListNode<>(obj);
        DoublyLinkedListNode<T> tail = head.prev;
        node.next = head;
        node.prev = tail;
        tail.next = node;
        head.prev = node;
        size++;
    }

    public void removeFront() {
        if (size > 0) {
            DoublyLinkedListNode<T> newHead = head.next.next;
            newHead.prev = head;
            head.next = newHead;
            size--;
        }
    }

    public void removeTail() {
        if (size > 0) {
            DoublyLinkedListNode<T> newTail = head.prev.prev;
            newTail.next = head;
            head.prev = newTail;
            size--;
        }
    }

    public T findFromHead(int i) {
        if (i < 0 || i >= size) {
            throw new IndexOutOfBoundsException();
        }
        DoublyLinkedListNode<T> node = head.next;
        while (i > 0 && node != head) {
            node = node.next;
            i--;
        }
        return node.item;
    }

    public T findFromTail(int i) {
        if (i < 0 || i >= size) {
            throw new IndexOutOfBoundsException();
        }
        DoublyLinkedListNode<T> node = head.prev;
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
        DoublyLinkedListNode node = head.next;
        while (node != head.prev) {
            str.append(String.valueOf(node.item));
            str.append(", ");
            node = node.next;
        }
        str.append(head.prev.item.toString());
        str.append("]");
        return str.toString();
    }

    public static void main(String[] args) {
        // DO NOT CHANGE THE FOLLOWING CODE.

        DoublyLinkedList<Integer> l = new DoublyLinkedList<>();
        System.out.println("### TESTING insertFront ###\nEmpty list is " + l);

        l.insertFront(9);
        System.out.println("\nInserting 9 at front.\nList with 9 is " + l);
        if (l.head.next.item != 9) {
            System.out.println("head.next.item is wrong.");
        }
        if (l.head.next.prev != l.head) {
            System.out.println("head.next.prev is wrong.");
        }
        if (l.head.prev.item != 9) {
            System.out.println("head.prev.item is wrong.");
        }
        if (l.head.prev.next != l.head) {
            System.out.println("head.prev.next is wrong.");
        }
        if (l.size != 1) {
            System.out.println("size is wrong.");
        }

        l.insertFront(8);
        System.out.println("\nInserting 8 at front.\nList with 8 and 9 is " + l);
        if (l.head.next.item != 8) {
            System.out.println("head.next.item is wrong.");
        }
        if (l.head.next.prev != l.head) {
            System.out.println("head.next.prev is wrong.");
        }
        if (l.head.prev.item != 9) {
            System.out.println("head.prev.item is wrong.");
        }
        if (l.head.prev.next != l.head) {
            System.out.println("head.prev.next is wrong.");
        }
        if (l.head.next.next != l.head.prev) {
            System.out.println("l.head.next.next != l.head.prev.");
        }
        if (l.head.prev.prev != l.head.next) {
            System.out.println("l.head.prev.prev != l.head.next.");
        }
        if (l.size != 2) {
            System.out.println("size is wrong.");
        }



        l = new DoublyLinkedList<>(1, 2);
        System.out.println("\n\n### TESTING removeFront ###\nList with 1 and 2 is " + l);

        l.removeFront();
        System.out.println("\nList with 2 is " + l);
        if (l.head.next.item != 2) {
            System.out.println("head.next.item is wrong.");
        }
        if (l.head.next.prev != l.head) {
            System.out.println("head.next.prev is wrong.");
        }
        if (l.head.prev.item != 2) {
            System.out.println("head.prev.item is wrong.");
        }
        if (l.head.prev.next != l.head) {
            System.out.println("head.prev.next is wrong.");
        }
        if (l.size != 1) {
            System.out.println("size is wrong.");
        }

        l.removeFront();
        System.out.println("\nEmpty list is " + l);
        if (l.head.next != l.head) {
            System.out.println("head.next is wrong.");
        }
        if (l.head.prev != l.head) {
            System.out.println("head.prev is wrong.");
        }
        if (l.size != 0) {
            System.out.println("size is wrong.");
        }

        l.removeFront();
        System.out.println("\nEmpty list is " + l);
        if (l.head.next != l.head) {
            System.out.println("head.next is wrong.");
        }
        if (l.head.prev != l.head) {
            System.out.println("head.prev is wrong.");
        }
        if (l.size != 0) {
            System.out.println("size is wrong.");
        }
    }

}
