package com.demofollow.demo;

import org.springframework.stereotype.Component;

// import jakarta.annotation.PostConstruct;

@Component
public class Parrot {
    private String name;

    public Parrot() {
        System.out.println("Parrot object created.");
    }

    // @PostConstruct
    // public void init() {this.name = "Kiki";}
    public void setName(String name) {this.name = name;}
    public String getName() {return name;}

    @Override
    public String toString() {
        return "Parrot: " + name;
    }
}
