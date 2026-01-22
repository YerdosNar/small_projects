public class Person {
    private String name = "Безымянный";
    private int age = 18;

    {
        System.out.println("Блок инициализации: name = " + name + ", age = " + age);
        age = 21;
    }

    public Person() {
        System.out.println("Конструктор без параметров: name = " + name + ", age = " + age);
    }

    public Person(String name, int age) {
        System.out.println("Конструктор с параметрами: name = " + name + ", age = " + age);
        this.name = name;
        this.age = age;
    }

    public void printInfo() {
        System.out.println("Person: name = " + name + ", age = " + age);
    }
}
