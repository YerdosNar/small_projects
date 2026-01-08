import java.util.Random;

public class Generate {
    static int length = 10;

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Password length default = 15");
        } else {
            if (args[0].equals("-l") || args[0].equals("--length")) {
                try {
                    length = Integer.parseInt(args[1]);
                } catch (NumberFormatException e) {
                    System.out.println("ERROR: Please input number after '-l/--length' flag!");
                }
                System.out.println("Length: " + length);
            }
        }

        char[] password = new char[length];
        Random ran = new Random();
        int choose = ran.nextInt(2);
        System.out.println("Choose: " + choose);
        if (choose == 1) {
            password[0] = (char)(65 + ran.nextInt(26));
        } else {
            password[0] = (char)(97 + ran.nextInt(26));
        }

        for(int i = 1; i < length; i++) {
            choose = ran.nextInt(3);
            System.out.print("Choose: " + choose);
            if(choose == 0) {
                password[i] = (char)(ran.nextInt(10) + 48);
                System.out.println(" Generated: " + password[i]);
            }
            else if (choose == 1) {
                password[i] = (char)(65 + ran.nextInt(26));
                System.out.println(" Generated: " + password[i]);
            }
            else if (choose == 2) {
                password[i] = (char)(97 + ran.nextInt(26));
                System.out.println(" Generated: " + password[i]);
            }
        }

        System.out.println("Your password: " + new String(password));
    }
}
