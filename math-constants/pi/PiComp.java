import java.math.BigInteger;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class PiComp {
    public static void main(String[] args) {
        try {
            if (args.length==0) {
                System.out.println("java PiComp binDouble");
                System.out.println("java PiComp binBig n");
                System.out.println("java PiComp binCust n");
            }

            String mode = args[0];
            int digits = args.length>1 ? Integer.parseInt(args[1]) : 0;

            BigInteger rBI = null;
            String rS = "";
            long[] rL = new long[1];
            double rD = 0;

            long t_start = System.currentTimeMillis();
            System.out.println(t_start);

            switch(mode) {
                case "binDouble":
                    rD = binomialDouble();
                    break;
                case "binBig":
                    rBI = binomialBig(digits);
                    break;
                case "binCust":
                    rL = binomialCust(digits);
                    break;
                case "binCustOpt1":
                    rL = binomialCustOpt1(digits);
                    break;
            }

            long t_end = System.currentTimeMillis();
            System.out.println("Exec time: "+(t_end-t_start)+" msec.");

            switch(mode) {
                case "binDouble":
                    System.out.println(rD);
                    break;
                case "binBig":
                case "binCust":
                case "binCustOpt1":
                    String filePath = "pi"+"_"+mode+"_"+digits+".txt";

                    try (FileOutputStream fos = new FileOutputStream(filePath)) {
                        if (mode.equals("binBig")) rS = rBI.toString();
                        else if (mode.equals("binCust") || mode.equals("binCustOpt1")) {
                            for (int i=0; i<rL.length; i++) {
                                rS += String.format("%09d", rL[i]);
                            }
                        }
                        byte[] bytes = rS.getBytes();
                        fos.write(bytes);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    break;
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static double binomialDouble() {
        double pi = 0;
        double fract5 = 16;
        double fract239 = 4;

        fract5 = 16.0/5;
        fract239 = 4.0/239;
        pi = fract5 - fract239;
        for (int i=1; i<20; i++) {
            fract5 /= 25;
            if ((i&1)>0) {
                pi -= fract5 / (i*2+1);
            }
            else {
                pi += fract5 / (i*2+1);
            }
            fract239 /= 57121;
            if ((i&1)>0) {
                pi += fract239 / (i*2+1);
            }
            else {
                pi -= fract239 / (i*2+1);
            }
        }
        return pi;
    }
    
    public static BigInteger binomialBig(int digits) {
        String base = "000000000";

        while (base.length()<digits+10) base = base + base;
        if (base.length()>digits+10) base = base.substring(0, digits+10);

        BigInteger pi = new BigInteger("0");
        BigInteger fract5 = new BigInteger("16"+base);
        BigInteger fract239 = new BigInteger("4"+base);
        BigInteger term25 = new BigInteger("25");
        BigInteger term57121 = new BigInteger("57121");
        BigInteger termOdd;
        int i = 1;

        fract5 = fract5.divide(new BigInteger("5"));
        fract239 = fract239.divide(new BigInteger("239"));
        pi = fract5.subtract(fract239);
        // 1 / 239^k will become zero faster thn 1/5^k
        while (fract239.compareTo(BigInteger.ONE) > 0) {
            fract239 = fract239.divide(term57121);
            fract5 = fract5.divide(term25);
            termOdd = new BigInteger(""+(2*i+1));
            if ((i&1)>0) {
                pi = pi.add(fract239.divide(termOdd));
                pi = pi.subtract(fract5.divide(termOdd));
            }
            else {
                pi = pi.add(fract5.divide(termOdd));
                pi = pi.subtract(fract239.divide(termOdd));
            }
            i++;
        }

        while (fract5.compareTo(BigInteger.ONE) > 0) {
            fract5 = fract5.divide(term25);
            termOdd = new BigInteger(""+(2*i+1));
            if ((i&1)>0) {
                pi = pi.subtract(fract5.divide(termOdd));
            }
            else {
                pi = pi.add(fract5.divide(termOdd));
            }
            i++;
        }
        return pi;
    }
    
    // array di long, ogni cella ha 9 cifre
    public static long[] binomialCust(int digits) {
        long BASE = 1000000000;

        int fract_length = digits / 9;
        while (fract_length%4!=0)fract_length++;

        long[] pi = new long[fract_length];
        long[] fract5 = new long[fract_length];
        long[] fract239 = new long[fract_length];
        long[] fract5div = new long[fract_length];
        long[] fract239div = new long[fract_length];
        long i2_1;
        fract5[0] =  160000000;
        fract239[0] = 40000000;
        int fract5_start=0;
        int fract239_start=0;
        long i=1;

        // 1/5 and 1/239
        fract5[0]/=5;
        long rem = 0;
        long rem2 = 0;
        for (int j=0; j<fract239.length; j++) {
            long n = rem*BASE + fract239[j];
            fract239[j] = n/239;
            rem = n%239;
        }
        // pi = 1/5 - 1/239
        rem = 0;
        for (int j=0; j<fract239.length; j++) {
            pi[j] += fract5[j] - fract239[j];
            if (pi[j]<0) {
                pi[j]+=BASE;
                pi[j-1]--;
            }
        }

        while (fract239_start<fract239.length-1 || fract239[fract239.length-1]>100)
        {
            i2_1 = (2*i+1);
            rem = 0;
            rem2 = 0;
            // fract239 /= 239*239
            for (int j=fract239_start; j<fract239.length; j++) {
                long n = rem*BASE + fract239[j];
                fract239[j] = n/57121;
                rem = n%57121;

                long n2 = rem2*BASE + fract239[j];
                fract239div[j] = n2/i2_1;
                rem2 = n2%i2_1;
            }
            rem = 0;
            rem2 = 0;
            // fract5 /= 5*5
            for (int j=fract5_start; j<fract5.length; j++) {
                long n = rem*BASE + fract5[j];
                fract5[j] = n/25;
                rem = n%25;
                
                long n2 = rem2*BASE + fract5[j];
                fract5div[j] = n2/i2_1;
                rem2 = n2%i2_1;
            }

            // pi = pi - fract5div + fract239div  => pi -= (fract5div - fract239div)
            if ((i&1)>0) {
                rem = 0;
                for (int j=fract5div.length-1; j>=fract5_start; j--) {
                    pi[j] += fract239div[j] - fract5div[j];
                    if (pi[j]<0) {
                        pi[j]+=BASE;
                        pi[j-1]--;
                    }
                    else if (pi[j]>=BASE) {
                        pi[j-1]+=pi[j]/BASE;
                        pi[j]%=BASE;
                    }
                }

                int j = fract5_start-1;
                while (j>0 && pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                    j--;
                }
            }
            // pi += fract5div - fract239div
            else {
                rem = 0;
                for (int j=fract5div.length-1; j>=fract5_start; j--) {
                    pi[j] += fract5div[j] - fract239div[j];
                    if (pi[j]<0) {
                        pi[j]+=BASE;
                        pi[j-1]--;
                    }
                    else if (pi[j]>=BASE) {
                        pi[j-1]+=pi[j]/BASE;
                        pi[j]%=BASE;
                    }
                }

                int j = fract5_start-1;
                while (j>0 && pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                    j--;
                }
            }

            if (fract239[fract239_start]==0) fract239_start++;
            if (fract5[fract5_start]==0) fract5_start++;

            i++;
        }

        while (fract5_start<fract5.length-1 || fract5[fract5.length-1]>100)
        {
            i2_1 = (2*i+1);
            rem = 0;
            rem2 = 0;
            // fract5 /= 5*5
            for (int j=fract5_start; j<fract5.length; j++) {
                long n = rem*BASE + fract5[j];
                fract5[j] = n/25;
                rem = n%25;
                long n2 = rem2*BASE + fract5[j];
                fract5div[j] = n2/i2_1;
                rem2 = n2%i2_1;
            }

            // pi = pi - fract5div
            if ((i&1)>0) {
                rem = 0;
                for (int j=fract5div.length-1; j>=fract5_start; j--) {
                    pi[j] -= fract5div[j];
                    if (pi[j]<0) {
                        pi[j]+=BASE;
                        pi[j-1]--;
                    }
                }

                int j = fract5_start-1;
                while (j>0 && pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                    j--;
                }
            }
            // pi += fract5div
            else {
                rem = 0;
                for (int j=fract5div.length-1; j>=fract5_start; j--) {
                    pi[j] += fract5div[j];
                    if (pi[j]<0) {
                        pi[j]+=BASE;
                        pi[j-1]--;
                    }
                    else if (pi[j]>=BASE) {
                        pi[j-1]+=pi[j]/BASE;
                        pi[j]%=BASE;
                    }
                }

                int j = fract5_start-1;
                while (j>0 && pi[j]>=BASE) {
                    pi[j-1]+=pi[j]/BASE;
                    pi[j]%=BASE;
                    j--;
                }
            }
            if (fract5[fract5_start]==0) fract5_start++;

            i++;
        }

        return pi;
    }

    // array di long, ogni cella ha 9 cifre
    public static long[] binomialCustOpt1(int digits) {
        long BASE = 1000000000;

        int fract_length = digits / 9;
        while (fract_length%4!=0)fract_length++;

        long[] e = new long[fract_length];
        long[] fract = new long[fract_length];
        e[0] = fract[0] = BASE/10;
        int fract_start=0;
        int i=1;

        while (fract_start<fract.length-1 || fract[fract.length-1]>100)
        {
            long rem = 0;
            // fract /= i
            for (int j=fract_start; j<fract.length; j+=4) {
                long n = rem*BASE + fract[j];
                fract[j] = n/i;
                rem = n%i;

                e[j] += fract[j];
                int k=j;
                while (e[k]>BASE) {
                    e[k-1] += e[k]/BASE;
                    e[k]%=BASE;
                }

                n = rem*BASE + fract[j];
                fract[j] = n/i;
                rem = n%i;
                
                e[j] += fract[j];
                k=j;
                while (e[k]>BASE) {
                    e[k-1] += e[k]/BASE;
                    e[k]%=BASE;
                }
            }

            if (fract[fract_start]==0) fract_start++;

            i++;
        }

        return e;
    }

    public static void printLong(long[] l) {
        for (int i=0; i<l.length; i++) {
            System.out.print(String.format("%09d", l[i]));
        }
        System.out.println("");
    }
}
