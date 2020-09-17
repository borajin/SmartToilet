import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
 
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.Date;
 
public class EchoServer {
    
    
    private ServerSocket     server_Soc; //��������
    private BufferedReader     br;            // Ŭ���̾�Ʈ�κ��� ���޹��� �޼����� �о���� ���۸޸𸮸� ���� ����
    private PrintWriter     pw;            // Ŭ���̾�Ʈ�� �޼����� ���� ����Ʈ ������
    private    Socket             soc;        // Ŭ���̾�Ʈ ����
    
    public EchoServer(){
        init();
    }
    
    public void init(){
        try{
            server_Soc = new ServerSocket(8888);
            System.out.println(getTime()+"Server is ready...");
            System.out.println(getTime()+"wait for client...");
            
            soc = server_Soc.accept();
            System.out.println("Client has accepted!!");
            
            br = new BufferedReader(new InputStreamReader(soc.getInputStream())); // Ŭ���̾�Ʈ�κ��� �����͸� �о�� �غ� �մϴ�
            pw = new PrintWriter(soc.getOutputStream());    // Ŭ���̾�Ʈ�� �����͸� ���� �غ� �մϴ�
            
            String readData = "";
            
            while(!(readData = br.readLine()).equals(null)){ //����ʿ��� ������ ���������� ��ٸ��ϴ�.
                 System.out.println(getTime()+"from Client > "+readData); //Ŭ�󸮾�Ʈ�� ���� �޼����� �н��ϴ�.
                 pw.println(readData); // ���� �޼��� �״�� Ŭ���̾�Ʈ ���� �����ϴ�.
                 pw.flush(); // ����Ʈ������ �޸𸮸� �ʱ�ȭ ��ŵ�ϴ�. �� �޼ҵ尡 �������� ���������� �����Ͱ� ���۵�
            }
            
            soc.close();
            
        }catch(SocketException | NullPointerException se){
            System.out.println("Ŭ���̾�Ʈ�� ������ �����Ͽ� ���α׷��� �����մϴ�...");
            System.exit(0);
        
        }catch(Exception e){
            e.printStackTrace();
        }
    }
    
    static String getTime(){
        SimpleDateFormat f = new SimpleDateFormat("[hh:mm:ss]");
        return f.format(new Date());
    }
    
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        
        new EchoServer();
 
    }
 
}