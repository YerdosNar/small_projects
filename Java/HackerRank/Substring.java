import java.util.Scanner;

public class Substring {

    public static String getSmallestAndLargest(String s, int k) {
        if(s.length() <= k) {
            return s+"\n"+s;
        }
        String smallest = "";
        String largest = "";
        smallest = s.substring(0, k);
        largest = s.substring(1, k+1);
        if(smallest.compareTo(largest) > 0) {
            String temp = largest;
            largest = smallest;
            smallest = temp;
        }

        // Complete the function
        // 'smallest' must be the lexicographically smallest substring of length 'k'
        // 'largest' must be the lexicographically largest substring of length 'k'
        for(int i = 2; i <= s.length()-k; i++) {
            String temp = s.substring(i, i+k);
            if(temp.compareTo(largest) > 0) {
                largest = temp;
            }
            if(smallest.compareTo(temp) > 0) {
                smallest = temp;
            }
        }

        return smallest + "\n" + largest;
    }


    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        String s = scan.next();
        int k = scan.nextInt();
        scan.close();

        System.out.println(getSmallestAndLargest(s, k));
    }
}
