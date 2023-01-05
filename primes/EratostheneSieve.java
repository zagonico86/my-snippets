public class EratostheneSieve
{
	public static int MODE_FACTOR = 1;
	public static int MODE_LIST = 2;
	
	public static void main(String[] args) { 
		long candidate = 1;
		int mode = -1;
		int limit = 1;
		
		// manage CLI arguments
		boolean showUsage = true;
		if (args.length==2) {
			if ("--factor".equals(args[0])) {
				mode = MODE_FACTOR;
				candidate = Long.parseLong(args[1]);
				limit = (int) Math.min(100000000, (int) Math.sqrt(candidate));
				showUsage = false;
			}
			else if ("--list".equals(args[0])) {
				mode = MODE_LIST;
				limit = Integer.parseInt(args[1]);
				showUsage = false;
			}
		}
		else if (args.length==3 && "--factor".equals(args[0])) {
			mode = MODE_FACTOR;
			candidate = Long.parseLong(args[1]);
			limit = (int) Double.parseDouble(args[2]);
			showUsage = false;
		}
		if (showUsage) {
			System.out.println("Usage:\n"+
			"\tjava EratostheneSieve --factor candidate [divisor_limit]\n"+
			"\t\ttrial division of candidate up to divisor_limit\n"+
			"\tjava EratostheneSieve --list limit\n"+
			"\t\tlist primes up to limit");
			return;
		}
		
		//
		if (mode == MODE_LIST) {
			System.out.println("List primes up to "+limit);
			int[] primes = eratostheneSieve(limit);
			
			for (int i=0; i<primes.length; i++) {
				System.out.println(primes[i]);
			}
		}
		else {
			System.out.println("Factor number "+candidate+" with limit "+limit);
			int[] primes = eratostheneSieve(limit);
			boolean factorFound = false;
			boolean willBeComplete = (limit >= (int) Math.sqrt(candidate));
			
			for (int i=0; i<primes.length && candidate>1; i++) {
				long remainder = candidate % primes[i];
				
				if (remainder==0) {
					System.out.println(primes[i]);
					candidate /= primes[i];
					i--; // try again this factor
					factorFound = true;
				}
			}
			
			if (factorFound) {
				if (willBeComplete) {
					if (candidate!=1) System.out.println(candidate);
					System.out.println("completely factored");
				}
				else {
					System.out.println("not factored part: "+candidate);
				}
			}
			else {
				System.out.println("no factor found");
			}
		}
	}

    public static int[] eratostheneSieve(int limit)
    {
		// perform the sieving
        boolean[] num = new boolean[limit+1];
        int lim = (int) Math.sqrt(limit);
        for (int i=2; i<=lim; ) {
            for (int j=i+i; j<=limit; j+=i) {
                num[j]=true;
            }
            i++;
            while (i<=limit && num[i]) i++;
        }

		// count the number of primes found
        int c=0;
        for (int i=2; i<=limit; i++) if (!num[i]) c++;

		// prep
        int[] ret = new int[c];
        c=0;
        for (int i=2; i<=limit; i++) {
            if (!num[i]) ret[c++]=i;
        }

        return ret;
    }
}
