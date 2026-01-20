import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class Generate {
    static Random ran = new Random();
    static final String RED = "\033[31m";
    static final String GRN = "\033[32m";
    static final String YEL = "\033[33m";
    static final String BLU = "\033[34m";
    static final String CYN = "\033[36m";
    static final String BLD = "\033[1m";
    static final String NOC = "\033[0m";

    static String generate(Password password) {
        char temp_pw[] = new char[password.length];

        int choose = ran.nextInt(2);
        if (choose == 0) {
            temp_pw[0] = (char)(ran.nextInt(26) + 65);
        }
        else if (choose == 1) {
            temp_pw[0] = (char)(ran.nextInt(26) + 97);
        }

        for (int i = 1; i < password.length; i++) {
            choose = ran.nextInt(3);
            if (choose == 0) {
                temp_pw[i] = (char)(ran.nextInt(10) + 48);
            }
            else if (choose == 1) {
                temp_pw[i] = (char)(ran.nextInt(26) + 65);
            }
            else if (choose == 2) {
                temp_pw[i] = (char)(ran.nextInt(26) + 97);
            }
        }

        return new String(temp_pw);
    }

    static Password parseArgs(String args[]) {
        int length = 15;
        String filename = "PasswordFile.txt";
        boolean write = false;

        for (int i = 0; i < args.length; i++) {
            if ((args[i].equals("-l") ||
                args[i].equals("--length")) &&
                i + 1 < args.length)
            {
                try {
                    length = Integer.parseInt(args[i+1]);
                    if (length < 5) {
                        System.out.println(YEL + "Minimum length: 5");
                        System.out.println(YEL + "Your length   : " + length + NOC);
                        System.out.println(BLU + "Setting legnth: 5" + NOC);
                        length = 5;
                    }
                    if (length > 80) {
                        System.out.println(YEL + "Maximum length: 80");
                        System.out.println(YEL + "Your length   : " + length);
                        System.out.println(BLU + "Setting legnth: 80" + NOC);
                        length = 80;
                    }
                } catch (NumberFormatException e) {
                    length = 15;
                    System.out.println(RED + "ERROR: " + e.getMessage() + NOC);
                }
            }
            else if (args[i].equals("-w") ||
                     args[i].equals("--write"))
            {
                write = true;
                if (i + 1 < args.length) {
                    filename = args[i+1];
                }
                else {
                    System.out.println(YEL + "WARNING: Filename is not set!" + NOC);
                    System.out.println(BLU + "Default: " + GRN + filename + NOC);
                }
            }
            else if (args[i].equals("-h") ||
                     args[i].equals("--help"))
            {
                System.out.println("Usage: java Generate [options]");
                System.out.println();
                System.out.println(BLU + "Options:");
                System.out.println("    -l/--length <number>      to set password length");
                System.out.println("    -w/--write <filename>     to save password in a file");
                System.out.println();
                System.out.println(GRN + "Example:");
                System.out.println("    java Generate -l 50 -w my_passwd.txt");
                System.out.println("        Generates password with length=50, and saves in " + BLU + "'my_passwd.txt'" + NOC);
                System.exit(0);
            }
        }

        if (write) {
            return new Password(length, write, filename);
        }
        else {
            return new Password(length, write);
        }
    }

    static void saveInFile(Password pw) {
        try {
            FileWriter fw = new FileWriter(pw.filename);
            fw.write(pw.password);
            fw.close();
        } catch (IOException e) {
            System.out.println(RED + "ERROR: " + e.getMessage() + NOC);
        }
    }

    static void printValues(Password pw) {
        System.out.println(CYN + BLD + "=====================");
        System.out.println(BLU + BLD + "| Length   : " + GRN + pw.length);
        System.out.println(BLU + BLD + "| Password : " + GRN + pw.password);
        if (pw.write) {
            System.out.println(BLU + BLD + "| Filename : " + GRN + pw.filename);
        }
        System.out.println(CYN + BLD + "=====================" + NOC);
    }

    public static void main(String[] args) {
        Password pw = parseArgs(args);
        pw.password = generate(pw);

        if (pw.write) {
            saveInFile(pw);
        }
        printValues(pw);
    }
}
