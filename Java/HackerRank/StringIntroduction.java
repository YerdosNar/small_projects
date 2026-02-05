import java.util.*;

public class StringIntroduction {
    public static void main(String[] args) {
        Scanner sc=new Scanner(System.in);
        String A=sc.next();
        String B=sc.next();

        String res = A.substring(0, 1).toUpperCase() +
                    A.substring(1) +
                    " " +
                    B.substring(0, 1).toUpperCase() +
                    B.substring(1);
        System.out.println(res.length() - 1);
        if(A.compareTo(B) > 0) {
            System.out.println("Yes");
        }
        else {
            System.out.println("No");
        }
        System.out.println(res);

        sc.close();
    }
}
