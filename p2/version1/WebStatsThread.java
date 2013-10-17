/**
 *
 * @author Chad
 */
public class WebStatsThread implements Runnable{
    private Webstats ws;
    private String fileName;

    public WebStatsThread(Webstats wS, String FileName) {
        ws = wS;
        fileName = FileName;
    }

    public void run() {
        try {
        ws.process_file((Object)this.fileName);
        } catch(Exception e){System.out.println(e);}
    }
}
