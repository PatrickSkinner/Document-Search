import java.util.Scanner;
import java.io.File;

public class Parser{
    public static void main(String [] args){
      Scanner sc = new Scanner(System.in);
      /*
      File file = new File("wsj.txt");
      try {
        sc = new Scanner(file);
      } catch (Exception ex){
        ex.printStackTrace();
      }*/
        int count = 0;
        while (sc.hasNext()){
            
            String token = sc.next();
            
            if (token.equals("</DOC>")){
                System.out.println("\n");
            }
            
            //OPENING TAG
            if (token.indexOf('<') == 0 && token.indexOf('/') != 1 && !token.equals("<DOC>") && !token.equals("<DOCNO>") && !token.equals("<TEXT>") && !token.equals("<P>") && !token.equals("<LP>")){
              token = sc.next(); 
              //WHILE NOT CLOSING TAG
              while (!token.contains("</")){
                  token = sc.next();             
                }
            } else {
            
              if (token.indexOf('<') != 0){
                if(token.indexOf('<') != -1 ){
                    token = token.substring(0,token.indexOf('<'));
                }
                String strippedToken = token.replaceAll("\\W", "").toLowerCase();
                
                if(!strippedToken.isEmpty()){
                  System.out.println(strippedToken);   
                  count++;
                }      
              }
            }
        }
    }
}