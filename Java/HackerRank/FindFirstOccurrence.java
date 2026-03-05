import java.io.*;
import java.util.*;
import java.util.stream.*;
import static java.util.stream.Collectors.toList;

class Result {

    /*
     * Complete the 'findFirstOccurrence' function below.
     *
     * The function is expected to return an INTEGER.
     * The function accepts following parameters:
     *  1. INTEGER_ARRAY nums
     *  2. INTEGER target
     */
    public static int findFirstOccurrence(List<Integer> nums, int target) {
    // Write your code here
        if (nums == null || nums.size() == 0) return -1;

        int low = 0, high = nums.size()-1;
        int result = -1;

        while(low <= high) {
            int mid = (low+high) / 2;

            if (nums.get(mid) < target) low=mid+1;
            if (nums.get(mid) > target) high=mid-1;

            if (nums.get(mid) == target) {
                result = mid;
                high = mid-1;
            }
        }

        return result;
    }

}

public class FindFirstOccurrence {
    public static void main(String[] args) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        int numsCount = Integer.parseInt(bufferedReader.readLine().trim());
        List<Integer> nums = IntStream.range(0, numsCount).mapToObj(i -> {
            try {
                return bufferedReader.readLine().replaceAll("\\s+$", "");
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
        })
            .map(String::trim)
            .map(Integer::parseInt)
            .collect(toList());

        int target = Integer.parseInt(bufferedReader.readLine().trim());
        int result = Result.findFirstOccurrence(nums, target);
        System.out.println(result);

        bufferedReader.close();
    }
}

