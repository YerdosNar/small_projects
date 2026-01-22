public class Main {
    public static void main(String[] args) {
        System.out.println("Создаём объект p1:");
        Person p1 = new Person();
        p1.printInfo();

        System.out.println("\nСоздаём объект p2:");
        Person p2 = new Person("Петя", 30);
        p2.printInfo();
    }
}
