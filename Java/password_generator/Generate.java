import java.util.Random;
import java.io.File;

public class Generate {
    static int length = 10;
    static String passwordFileName;

    static String generatePassword() {
        char[] password = new char[length];
        Random ran = new Random();
        int choose = ran.nextInt(2);
        if (choose == 1) {
            password[0] = (char)(65 + ran.nextInt(26));
        } else {
            password[0] = (char)(97 + ran.nextInt(26));
        }

        for(int i = 1; i < length; i++) {
            choose = ran.nextInt(3);
            if(choose == 0) {
                password[i] = (char)(ran.nextInt(10) + 48);
            }
            else if (choose == 1) {
                password[i] = (char)(65 + ran.nextInt(26));
            }
            else if (choose == 2) {
                password[i] = (char)(97 + ran.nextInt(26));
            }
        }

        return new String(password);
    }

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Password length default=" + length);
        } else {
            for (int i = 0; i < args.length; i++) {
                if (args[i].equals("-l") || args[i].equals("--length") && i+1 < args.length) {
                    try {
                        length = Integer.parseInt(args[i+1]);
                    } catch (NumberFormatException e) {
                        System.out.println("ERROR: Please input number after '-l/--length' flag!");
                    }
                    System.out.println("Length: " + length);
                }
                else if(args[i].equals("-w") || args[i].equals("--write")) {
                    if (i+1 < args.length) {
                        passwordFileName = args[i+1];
                    } else {
                        passwordFileName = "passwordFile.txt";
                    }
                    try {
                        File passwordFile = new File(passwordFileName);
                        if (passwordFile.exists() && !passwordFile.isDirectory()) {
                            System.out.println("Password File exists, overwriting...");
                        } else {
                            passwordFile.createNewFile();
                        }
                        System.out.println("Password File: " + passwordFileName);
                    } catch (Exception e) {
                        System.out.println("ERROR: " + e.getMessage());
                    }
                }
            }
        }

        String password = generatePassword();
        System.out.println("Your password: " + new String(password));
    }
}
