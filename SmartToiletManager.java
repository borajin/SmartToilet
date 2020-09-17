import java.awt.EventQueue;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;


public class SmartToiletManager extends JFrame {
	ServerSocket server_Soc;
	BufferedReader br; 
	Socket soc;

	JTextArea MessageTextArea;
	
	public SmartToiletManager() {
		setTitle("SmartToilet Server");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);	//x��ư������ �� ���μ������� Ȯ���ϰ� ���� �ϴ� �ڵ�
		setBounds(100, 100, 450, 500);
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		
		JPanel ServerPanel = new JPanel();
		ServerPanel.setLayout(new BoxLayout(ServerPanel, BoxLayout.Y_AXIS));
		
		JPanel ButtonPanel = new JPanel();
		ButtonPanel.setLayout(new BoxLayout(ButtonPanel, BoxLayout.X_AXIS));
		
		JButton startButton = new JButton("���� �����ϱ�");
		ButtonPanel.add(startButton);
		startButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				Thread thread = new Thread() {
					public void run() {
						init();
					}
				};
				thread.start();
			}
		});
				
		JButton stopButton = new JButton("���� ����");
		ButtonPanel.add(stopButton);
		stopButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				stopServer();
			}
		});
		
		ServerPanel.add(ButtonPanel);
		
		JScrollPane MessageScrollPane = new JScrollPane();
		ServerPanel.add(MessageScrollPane);

		MessageTextArea = new JTextArea();
		MessageTextArea.setRows(4);
		MessageTextArea.setFont(new Font("����", Font.BOLD, 20));
		MessageScrollPane.setViewportView(MessageTextArea);

		this.add(ServerPanel);
	}
	
    public void init(){
        try{
            server_Soc = new ServerSocket(8888);
            MessageTextArea.append(getTime()+"Server is ready...\n");
            
            while(true) {
            	soc = server_Soc.accept();
            
            	br = new BufferedReader(new InputStreamReader(soc.getInputStream())); 
            
            	String readData = br.readLine()
            			;
            
            	MessageTextArea.append(getTime()+"Client > "+ readData + "\n");

            	soc.close();
            }
        } catch(SocketException | NullPointerException se){
            System.exit(0);
        } catch(Exception e){
            e.printStackTrace();
        }
    }
    
    static String getTime(){
        SimpleDateFormat f = new SimpleDateFormat("[hh:mm:ss]");
        return f.format(new Date());
    }
	
	public void stopServer() {
		try {
			if(server_Soc != null && !server_Soc.isClosed()) {
				server_Soc.close();
			}
			MessageTextArea.append("���� ���� . . .\n");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String [] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
					SmartToiletManager manager = new SmartToiletManager ();
					manager.setVisible(true);
			}
		});
	}
}