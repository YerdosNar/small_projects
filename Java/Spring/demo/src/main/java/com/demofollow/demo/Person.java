package com.demofollow.demo;

import org.springframework.beans.factory.annotation.Qualifier;
// import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class Person {

    private String name = "Ella";
    private final Parrot parrot;

    public Person(
        @Qualifier("parrot2") Parrot parrot) {
        this.parrot = parrot;
    }
    // getter setter
    public void setName(String name) {this.name = name;}
    public String getName() {return name;}

    // @Autowired
    // public void setParrot(Parrot parrot) {this.parrot=parrot;}
    public Parrot getParrot() {return parrot;}

    @Override
    public String toString() {
        return "Person: "+name;
    }
}
