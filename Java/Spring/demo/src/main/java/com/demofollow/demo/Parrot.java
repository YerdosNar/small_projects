package com.demofollow.demo;

// import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

// import jakarta.annotation.PostConstruct;

@Component
public class Parrot {
    private String name = "Kiki";

    // SECTION 2 START
    // @PostConstruct
    // public void init() {this.name = "Kiki";}
    // SECTION 2 END
    public void setName(String name) {this.name = name;}
    public String getName() {return name;}

    @Override
    public String toString() {
        return "Parrot: " + name;
    }
}
