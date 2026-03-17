package com.demofollow.demo;

// import java.util.List;
// import java.util.function.Supplier;

import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

@SpringBootApplication
public class Application {

	public static void main(String[] args) {
        var context =
            new AnnotationConfigApplicationContext(
                ProjectConfig.class);

        // SECTION 2 START
        //
        // Parrot p = context.getBean(Parrot.class);
        // System.out.println(p);
        // System.out.println("Primary Parrot: "+p.getName());
        //
        // List<Parrot> list = context.getBean(List.class);
        // for (int i = 0; i < list.size(); i++) {
        //     System.out.println(list.get(i).getName());
        // }
        //
        // Parrot x = new Parrot();
        // x.setName("Kiki");
        //
        // Supplier<Parrot> parrotSupplier = () -> x;
        // context.registerBean("parrot1",
        //                     Parrot.class,
        //                     parrotSupplier,
        //                     bc -> bc.setPrimary(true));
        //
        // Parrot p = context.getBean(Parrot.class);
        // System.out.println(p.getName());
        //
        // SECTION 2 END

        // SECTION 3 START
        Person person = context.getBean(Person.class);

        System.out.println("Person's name: " + person.getName());
        System.out.println(person.getName() + "'s parrort: " + person.getParrot());
        // SECTION 3 END

        context.close();
	}

}
