import java.math.BigInteger;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class EulerComp {
    public static void main(String[] args) {
        try {
            if (args.length==0) {
                System.out.println("java EulerComp binDouble");
                System.out.println("java EulerComp binBig n");
                System.out.println("java EulerComp binCust n");
            }

            String mode = args[0];
            int digits = args.length>1 ? Integer.parseInt(args[1]) : 0;

            BigInteger rBI = null;
            String rS = "";
            long[] rL = new long[1];
            double rD = 0;

            long t_start = System.currentTimeMillis();

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
                    String filePath = "e"+"_"+mode+"_"+digits+".txt";

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
        double e = 1;
        double fract = 1;

        for (int i=1; i<20; i++) {
            fract /= i;
            e += fract;
        }
        return e;
    }
    
    public static BigInteger binomialBig(int digits) {
        String base = "000000000";

        while (base.length()<digits+10) base = base + base;
        if (base.length()>digits+10) base = base.substring(0, digits+10);

        BigInteger e = new BigInteger("1"+base);
        BigInteger fract = new BigInteger("1"+base);
        int i = 1;

        while (fract.compareTo(BigInteger.ONE) > 0) {
            fract = fract.divide(BigInteger.valueOf(i));
            e = e.add(fract);
            i++;
        }
        return e;
    }
    
    // array di long, ogni cella ha 9 cifre
    public static long[] binomialCust(int digits) {
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
            for (int j=fract_start; j<fract.length; j++) {
                long n = rem*BASE + fract[j];
                fract[j] = n/i;
                rem = n%i;
            }
            // e += fract
            for (int j=fract_start; j<fract.length; j++) {
                e[j] += fract[j];
                int k=j;
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
}