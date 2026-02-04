import java.util.Scanner;

public class Anagrams {

    static boolean isAnagram(String a, String b) {
        // Complete the function
        if(a.length() != b.length()) {
            return false;
        }
        int a_freq[] = new int[26];
        int b_freq[] = new int[26];
        a = a.toLowerCase();
        b = b.toLowerCase();
        int length = a.length();
        for(int i = 0; i < length; i++) {
            a_freq[a.charAt(i)-'a']++;
            b_freq[b.charAt(i)-'a']++;
        }
        for(int i = 0; i < a_freq.length; i++) {
            if(a_freq[i] != b_freq[i]) {
                return false;
            }
        }
        return true;
    }

    public static void main(String[] args) {

        Scanner scan = new Scanner(System.in);
        String a = scan.next();
        String b = scan.next();
        scan.close();
        boolean ret = isAnagram(a, b);
        System.out.println( (ret) ? "Anagrams" : "Not Anagrams" );
    }
}
