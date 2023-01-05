import java.math.BigInteger;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class PollardRho
{	
	public static void main(String[] args) { 
		BigInteger candidate=BigInteger.ONE, a=BigInteger.ONE, x=BigInteger.valueOf(2L);
		int timeLimit=120;
		
		// manage CLI arguments
		boolean showUsage = true;
		switch (args.length) {
			case 4:
				timeLimit = Integer.parseInt(args[3]);
			case 3:
				x = new BigInteger(args[2]);
			case 2:
				a = new BigInteger(args[1]);
			case 1:
				candidate = new BigInteger(args[0]);
		}
		
		if (args.length == 0 || args.length > 4) {
			System.out.println("Usage:\n"+
			"\tjava PollardRho candidate [a=1] [x=2] [timeLimit=120]\n"+
			"\t\tapply Pollard's Rho method to candidate with polynomial f(z) = z^2+a from z=x");
			return;
		}
		
		PollardRho.pollardRho(candidate, a, x, timeLimit);
	}

    public static void pollardRho(BigInteger candidate, BigInteger a, BigInteger x, int timeLimit) {
        long last_update = System.currentTimeMillis();
		long startTime = System.currentTimeMillis();

		System.out.println(
                getCurrentlyDateTime()+": starting with parameters:\n"+
                "\t- time limit: "+ timeLimit+"\n"+
                "\t- a: "+ a+"\n"+
                "\t- x: "+ x);

        BigInteger step1, step2, res;

        int count=0;

        do {
            step1 = x.multiply(x).add(a).mod(candidate);
            step2 = step1.multiply(step1).add(a).mod(candidate);

            res = candidate.gcd(step2.subtract(step1).abs());

            if (candidate.equals(res)) {
                System.out.println(getCurrentlyDateTime()+": "+"STOPPED: bad polynomial");
                break;
            }
            count++;

            if (timeLimit>0 && count%10==0) {
                long time = System.currentTimeMillis();
                if (time-startTime > timeLimit* 1000L) {
                    System.out.println(getCurrentlyDateTime()+": "+"STOPPED: reached time limit");
                    System.out.println(count+" x evalueated");
                    break;
                }
                else if (time - last_update > 20000) {
                    System.out.println(getCurrentlyDateTime()+": "+ count+" x evalueted...");
                    last_update = time;
                }
            }

            x = x.add(BigInteger.ONE);
        } while (res.equals(BigInteger.ONE));

		BigInteger factors[];
        if (!res.equals(BigInteger.ONE)) {
			factors = new BigInteger[2];
            factors[0] = res;
            factors[1] = candidate.divide(res);
        }
        else {
			factors = new BigInteger[1];
            factors[0] = candidate.divide(res);
        }
		
		System.out.println(getCurrentlyDateTime()+": factors found:");
		
        for (BigInteger factor : factors) {
            System.out.println((factor.isProbablePrime(5) ? "pp" : "c")+factor);
        }
    }
	
	public static String getCurrentlyDateTime() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
        return dateFormat.format(new Date());
    }
}
