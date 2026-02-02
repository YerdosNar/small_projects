import java.util.Scanner;

public class Shell {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String homeDir = System.getProperty("user.home");
        String rawPath = System.getProperty("user.dir");
        String path = rawPath.replace(homeDir, "~");
        String userName = System.getProperty("user.name");
        System.out.print(userName + path + "$ ");

        String rawLine = sc.nextLine();
        String[] tokens = rawLine.split(" ");
        for(int i = 0; i < tokens.length; i++) {
            System.out.println(tokens[i]);
        }

        sc.close();
    }
}
