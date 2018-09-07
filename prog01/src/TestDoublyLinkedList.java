public class TestDoublyLinkedList {

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
        System.out.println();

        // find cases

        System.out.println("TESTING find cases");
        l = new DoublyLinkedList<>();
        l.insertBack(0);
        l.insertBack(1);
        l.insertBack(2);
        l.insertBack(3);
        l.insertBack(4);
        l.insertBack(5);

        System.out.println(l);
        System.out.println("first element from head = 0: " + l.findFromHead(0));
        System.out.println("third element from tail = 2: " + l.findFromTail(3));

        System.out.println("6th element from head should throw error");
        try {
            System.out.println(l.findFromHead(6));
            System.out.println("6th element from head failed to throw error");
        } catch (IndexOutOfBoundsException e) {
            System.out.println("6th element did throw error");
            e.printStackTrace();
        }
    }
}
