Program AMAIL;
Uses crt,sysutils,dos;

Type
    str16=string[16];
    str8=string[8];
    str20=string[20];
    str73=string[73];
    TregUsuarios=record
                   Id_Usuario: str16;
                   Password: str8;
                   PosIni: integer;
    				  end;
    TarchUsuarios=file of TregUsuarios;
    TregUsuarios2=record
                    Id_Usuario: str16;
                    Password: str8;
                    PosArch: byte;
   				  	end;				
    TvecUsuarios=array [1..100] of TregUsuarios2;
    TregMensajes=record
                   Id_Usuario: str16;
                   Fecha: longint;
                   Destinatario: str16;
                   Asunto: str20;
                   Mensaje: str73;
                   Senial: char;
    				  end;
    TarchMensajes=file of TregMensajes;
    Tinfo=record
				 Fecha:longint;
				 Usuario:str16;
				 Posarch:byte;
			  end;	 
    Tlista=^Tnodo;
    Tnodo=record
    			info:Tinfo;
    			sgte:Tlista;
			 end;	

Function ObtenerClave(): str8;

         Var
            CADENA: str8;
            N: integer;
            CARACTER: char;
            
         begin
				N:= 1;
		      CADENA:= '';
		      CARACTER:= readkey;
            while (N<=8) and (CARACTER<>chr(13)) do
			   	begin
				   	if (N>1) and (N<=8) and (CARACTER=chr(8))
							then begin
								  write(chr(8));
								  write(' ');
								  write(chr(8));
								  Dec(N);
								  end
                  	else begin
	               	 	  cadena[N]:= CARACTER;
								  write('*');
								  inc(N);
							  	  end;
               	if (N<=8)
					   	then	CARACTER:= readkey;
            	end;
              	CADENA[0]:= chr(n-1);
		         ObtenerClave:= CADENA;
         end;

Procedure Burbujeo (var vec: TvecUsuarios; n: byte);

          Var
             i,j: byte;
             bol: boolean;
             aux: TregUsuarios2;

          begin
          	j:= n-1;
            repeat
               bol:= true;
               for i:=1 to j do
               	begin
                  	if vec[i].Id_Usuario>vec[i+1].Id_Usuario
                     	then begin
                        	  aux:= vec[i];
                             vec[i]:= vec[i+1];
                             vec[i+1]:= aux;
                             bol:= false;
                          	  end;
                  end;
               j:= j-1;
            until (bol= true);
          end;

Procedure BusBinariVec (vec: TvecUsuarios; n: byte; x: str16;  var pos: shortint);

          Var
             li,ls,med: byte;

          begin
          	li:= 1;
            ls:= n;
            pos:= -1;
            while (li<=ls) and (pos=-1) do
            	begin
               	med:= (li+ls) div 2;
                  if vec[med].Id_Usuario=x
                  	then pos:= med
                     else begin
                     	  if vec[med].Id_Usuario>x
                          then ls:= med-1
                          else li:= med+1;
                          end;
               end;
          end;

Procedure InsertNodOrdRemDest(var Lista:Tlista; Rinfo:Tinfo);
	
			Var
			nuevo,P,Q:Tlista;
			
			begin
				new(nuevo);
				nuevo^.info:=Rinfo;
				P:=Lista;
				while (P<>NIL) and (P^.info.Usuario<Rinfo.Usuario) do
					begin
						Q:=P;
						P:=P^.sgte;
					end;
				if Lista=P
					then begin	
							nuevo^.sgte:=Lista;
							Lista:=nuevo;
						  end
					else begin
							nuevo^.sgte:=P;
							Q^.sgte:=nuevo;
						  end;  
			end;

Procedure InsertNodOrdFechDes(var Lista:Tlista; Rinfo:Tinfo);
	
			Var
			nuevo,P,Q:Tlista;
			
			begin
				new(nuevo);
				nuevo^.info:=Rinfo;
				P:=Lista;
				while (P<>NIL) and (P^.info.Fecha>Rinfo.Fecha) do
					begin
						Q:=P;
						P:=P^.sgte;
					end;
				if Lista=P
					then begin	
							nuevo^.sgte:=Lista;
							Lista:=nuevo;
						  end
					else begin
							nuevo^.sgte:=P;
							Q^.sgte:=nuevo;
						  end;  
			end;

Procedure InsertNodOrdFechAsc(var Lista:Tlista; Rinfo:Tinfo);
	
			Var
			nuevo,P,Q:Tlista;
			
			begin
				new(nuevo);
				nuevo^.info:=Rinfo;
				P:=Lista;
				while (P<>NIL) and (P^.info.Fecha<Rinfo.Fecha) do
					begin
						Q:=P;
						P:=P^.sgte;
					end;
				if Lista=P
					then begin	
							nuevo^.sgte:=Lista;
							Lista:=nuevo;
						  end
					else begin
							nuevo^.sgte:=P;
							Q^.sgte:=nuevo;
						  end;  
			end;

Procedure Validar (var vec: TvecUsuarios; user: str16; pass: str8; var bol: boolean; var n: byte; var pos: shortint);

          Var
             reg: TregUsuarios;
             regaux: TregUsuarios2;
             AUSUARIOS: TarchUsuarios;

          begin
               assign (AUSUARIOS,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AUSUARIOS.dat');
               reset (AUSUARIOS);
               n:= 1;
               while not eof(AUSUARIOS) do
               	begin
                  read (AUSUARIOS,reg);
                  regaux.Id_Usuario:= reg.Id_Usuario;
                  regaux.Password:= reg.Password;
                  regaux.PosArch:= n-1;
                  vec[n]:= regaux;
                  n:= n+1;
                  end;
               close (AUSUARIOS);
               n:= n-1;
               pos:= -1;
               Burbujeo (vec,n);
               BusBinariVec (vec,n,user,pos);
               if (pos<>-1) and (pass=vec[pos].Password)
                  then bol:= true
                  else bol:= false;
          end;

Procedure Redactar (vec: TvecUsuarios; n: byte; pos: shortint);

          Var
             x: char;
             posd: shortint;
             rmensajes: TregMensajes;
             year,month,day,weekday: word;
             flag: boolean;
             AMENSAJES: TarchMensajes;
             AUSUARIOS: TarchUsuarios;
             RegUsuarios: TregUsuarios;

          begin
          	repeat
		      	clrscr;
          		flag:= false;
		      	writeln ('         Bienvenido a Amail2011           La vision de correo electronico de AyE');
		      	writeln();
		     	 	writeln ('De: ');
		     	 	writeln();
		      	writeln ('Para: ');
		      	writeln();
		      	writeln ('Asunto: ');
		      	writeln();
		      	writeln ('Mensaje: ');
		      	writeln();
		      	writeln ('    1- Enviar           2- Cancelar');
          		gotoxy (5,3);
          		readln (rmensajes.Id_Usuario);
          		gotoxy (7,5);
          		readln (rmensajes.Destinatario);
         		gotoxy (9,7);
          		readln (rmensajes.Asunto);
          		gotoxy (10,9);
          		readln (rmensajes.Mensaje);
          		gotoxy (3,11);
		      	x:= readkey;
            	case x of
            		'1': begin
               			BusBinariVec (vec,n,rmensajes.Destinatario,posd);
                        if posd<>-1
                        	then begin
                        		  assign (AMENSAJES,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AMENSAJES.dat');
                        		  reset(AMENSAJES);
                        		  if eof(AMENSAJES)                     	
                                		then rewrite(AMENSAJES)
                                		else seek(AMENSAJES,filesize(AMENSAJES));                      
                                assign (AUSUARIOS,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AUSUARIOS.dat');
                                reset(AUSUARIOS);
                           	  getdate(year,month,day,weekday);
                                rmensajes.Fecha:= (year*10000+month*100+day);
                                rmensajes.Senial:= 'E';
                                write (AMENSAJES,rmensajes);
                                seek (AUSUARIOS,vec[pos].PosArch);
                                read (AUSUARIOS,RegUsuarios);
                                seek (AUSUARIOS,vec[pos].PosArch);
                                if RegUsuarios.PosIni=-1
                                		then begin
                                			  RegUsuarios.PosIni:= filesize (AMENSAJES)-1;
                                			  write(AUSUARIOS,RegUsuarios);
                                			  end;		
                                rmensajes.Senial:= 'R';
                                write (AMENSAJES,rmensajes);
                                seek (AUSUARIOS,vec[posd].PosArch);
                                read (AUSUARIOS,RegUsuarios);
                                seek (AUSUARIOS,vec[posd].PosArch);
                                if RegUsuarios.PosIni=-1
                                		then begin
													  RegUsuarios.PosIni:= filesize (AMENSAJES)-1;
                                		 	  write(AUSUARIOS,RegUsuarios);
                                			  end;
                                close(AUSUARIOS);
               		  			  close(AMENSAJES);  					  
                                end			  		
                          else begin
                          		 writeln ('El destinatario no existe. Presione ENTER para continuar.');
                          		 readln();
                          		 end; 
               		  end;
               	'2': flag:= true;
            end;
          	until flag;
          end;

Procedure VerMensaje(var AMENSAJES: TarchMensajes; Lista: Tlista; NRO: byte; USER:str16);

		Var
			flag: boolean;
			x: char;
			rmensajes: TregMensajes;
		
		begin
			repeat
				clrscr;
				flag:=false;
         	writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
         	writeln ();
         	writeln ('Asunto: ');
         	writeln ();
         	writeln ('De: ');
         	writeln ();
         	writeln ('Para: ');
         	writeln ();
         	writeln ('Mensaje: ');
        	 	writeln ();
         	writeln ();
         	writeln ();
         	writeln ('           M- volver al menu anterior');
         	gotoxy (9,3);
				seek(AMENSAJES,Lista^.info.Posarch);
				read(AMENSAJES,rmensajes);
				write(rmensajes.Asunto);
         	gotoxy (5,5);
				write(Lista^.info.Usuario);
         	gotoxy (7,7);
				write(USER);
        	 	gotoxy (10,9);
				write(rmensajes.Mensaje);
         	gotoxy (10,13);
				x:= readkey;	
			until x='m'
		end;

Procedure Opcion3Enviados (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Id_Usuario=USER) and (rmensajes.Senial='E')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Destinatario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdRemDest(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        Para                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure Opcion2Enviados (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Id_Usuario=USER) and (rmensajes.Senial='E')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Destinatario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdFechDes(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        Para                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure Opcion1Enviados (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Id_Usuario=USER) and (rmensajes.Senial='E')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Destinatario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdFechAsc(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        Para                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure MenuEnviados (vec: TvecUsuarios; pos: shortint);
	 
			 Var
			 	AMENSAJES: TarchMensajes;
			 	rmensajes: TregMensajes;
			 	AUSUARIOS: TarchUsuarios;
			 	RegUsuarios: TregUsuarios;
			 	flag: boolean;
			 	x: char;

			 begin
			 	flag:=false;
			 	assign (AMENSAJES,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AMENSAJES.dat');
            reset(AMENSAJES);
            assign (AUSUARIOS,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AUSUARIOS.dat');
            reset(AUSUARIOS);
			 	repeat
			 		clrscr;
			 		writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			 		writeln();
			 		writeln('                   Mostrar Recibidos');
			 		writeln();
			 		writeln('                   1- Ordenado por fecha ascendente');
			 		writeln('                   2- Ordenado por fecha descendente');
			 		writeln('                   3- Ordenado por Destinatario');
			 		writeln('                   4- Volver al menu principal');
			 		x:=readkey;
			 		case x of
			 		'1': Opcion1Enviados (AMENSAJES,AUSUARIOS,vec,pos);		 						
			 		'2': Opcion2Enviados (AMENSAJES,AUSUARIOS,vec,pos);
			 		'3': Opcion3Enviados (AMENSAJES,AUSUARIOS,vec,pos);
			 		'4': begin		 					
			 				close(AMENSAJES);
			 				close(AUSUARIOS);
			 				flag:=true;
			 			  end;	
			 		end;		 		
				until flag
				end;

Procedure Opcion3Recibidos (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Destinatario=USER) and (rmensajes.Senial='R')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Id_Usuario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdRemDest(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        De                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure Opcion2Recibidos (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Destinatario=USER) and (rmensajes.Senial='R')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Id_Usuario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdFechDes(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        De                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure Opcion1Recibidos (var AMENSAJES: TarchMensajes; var AUSUARIOS: TarchUsuarios; vec: TvecUsuarios; pos: shortint);
		
		var
      	RegUsuarios: TregUsuarios;
         rmensajes: TregMensajes;
         Lista,P: Tlista;
         Rinfo: Tinfo;
         NO,Y,anio: word;
         dia,mes,I,NRO: byte;
         x: char;
			flag: boolean;
			USER:str16;
		
		begin
		 repeat	
			clrscr;
			flag:=false;
			Lista:=NIL;
			seek (AUSUARIOS,vec[pos].PosArch);
         read (AUSUARIOS,RegUsuarios);
         USER:=RegUsuarios.Id_Usuario;
			seek(AMENSAJES,RegUsuarios.PosIni);
			while not eof(AMENSAJES) do
				begin
					read(AMENSAJES,rmensajes);
					if (rmensajes.Destinatario=USER) and (rmensajes.Senial='R')
						then begin
			 					Rinfo.Fecha:=rmensajes.Fecha;
			 					Rinfo.Usuario:=rmensajes.Id_Usuario;
			 					Rinfo.Posarch:=filepos(AMENSAJES)-1;
			 					InsertNodOrdFechAsc(Lista,Rinfo);
			 				  end;
				end;
			writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			writeln();
			writeln ('Nro de Orden        De                 Asunto                 Fecha');
			Y:=4;
			NO:=1;
			P:=Lista;
			for I:=1 to 3 do
				begin	
					while P<>NIL do
						begin
							gotoxy(5,Y); 
							write(NO);
							gotoxy(18,Y);
							write(P^.info.Usuario);
							gotoxy(40,Y);
							seek(AMENSAJES,P^.info.Posarch);
							read(AMENSAJES,rmensajes);
							write(rmensajes.Asunto);
							gotoxy(60,Y);
							anio:=Rinfo.Fecha div 10000;
							mes:=(Rinfo.Fecha mod 10000) div 100;
							dia:=(Rinfo.Fecha mod 10000) mod 100;
							write(dia,'/',mes,'/',anio);
							Y:=Y+1;
							P:=P^.sgte;
							NO:=NO+1;
						end;
				end;		
			writeln();
			writeln();
			writeln ('nn- digite numero de mensaje a ver                  M- volver al menu anterior');
			x:=readkey;
			case x of
				'1': begin
						NRO:=1;
						VerMensaje(AMENSAJES,Lista,NRO,USER);
					  end;	
				'2': begin
						NRO:=2;
						P:=Lista^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;	
				'3': begin
						NRO:=3;
						P:=Lista;
						for I:=1 to 2 do
 							P:=P^.sgte;
						VerMensaje(AMENSAJES,P,NRO,USER);
					  end;				
				'm': flag:=true;
			end;
		 until flag			
		end;

Procedure MenuRecibidos (vec: TvecUsuarios; pos: shortint);
	 
			 Var
			 	AMENSAJES: TarchMensajes;
			 	rmensajes: TregMensajes;
			 	AUSUARIOS: TarchUsuarios;
			 	RegUsuarios: TregUsuarios;
			 	flag: boolean;
			 	x: char;

			 begin
			 	flag:=false;
			 	assign (AMENSAJES,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AMENSAJES.dat');
            reset(AMENSAJES);
            assign (AUSUARIOS,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AUSUARIOS.dat');
            reset(AUSUARIOS);
			 	repeat
			 		clrscr;
			 		writeln ('Bienvenido a Amail2011                   La vision de correo electronico de AyE');
			 		writeln();
			 		writeln('                   Mostrar Recibidos');
			 		writeln();
			 		writeln('                   1- Ordenado por fecha ascendente');
			 		writeln('                   2- Ordenado por fecha descendente');
			 		writeln('                   3- Ordenado por Remitente');
			 		writeln('                   4- Volver al menu principal');
			 		x:=readkey;
			 		case x of
			 		'1': Opcion1Recibidos (AMENSAJES,AUSUARIOS,vec,pos);		 						
			 		'2': Opcion2Recibidos (AMENSAJES,AUSUARIOS,vec,pos);
			 		'3': Opcion3Recibidos (AMENSAJES,AUSUARIOS,vec,pos);
			 		'4': begin		 					
			 				close(AMENSAJES);
			 				close(AUSUARIOS);
			 				flag:=true;
			 			  end;	
			 		end;		 		
				until flag
				end;
							  
Procedure Cuenta (vec: TvecUsuarios; n: byte; pos: shortint);

          Var
             x: char;
             flag:boolean;

          begin
          	flag:=false;
          	repeat
          		clrscr;
            	writeln ('1. Redactar');
            	writeln ('2. Recibidos');
            	writeln ('3. Enviados');
            	writeln ('4. Salir');
            	x:= readkey;
            	case x of
            		'1': Redactar (vec,n,pos);
               	'2': MenuRecibidos(vec,pos);
               	'3': MenuEnviados(vec,pos);
               	'4': flag:=true;        	
            	end;
          	until flag
          	end;

Procedure Menulogueo;

          Var
             x: char;
             user: str16;
             pass: str8;
             vec: TvecUsuarios;
             bol,flag: boolean;
             n: byte;
             pos: shortint;

          begin
          	repeat
               clrscr;
               flag:= false;
               writeln ('Bienvenido a Amail2011');
               writeln ('La vision de correo electronico de AyE                Acceder a Amail2011 con su');
               writeln ('                                                           Cuenta AyE');
               writeln ();
               writeln ('                                        Nombre de Usuario: ');
               writeln ();
               writeln ('                                        Password: ');
               writeln ();
               writeln ();
               writeln ('                                         1.Acceder   2.Volver al menu anterior');
               gotoxy (60,5);
               readln (user);
               gotoxy (51,7);
               pass:= ObtenerClave();
               gotoxy (40,10);
               x:= readkey;
               case x of
                    '1': begin
                         	Validar (vec,user,pass,bol,n,pos);
                         	if bol=true
                            then Cuenta (vec,n,pos)
                            else begin
                                 writeln('Usuario y/o contrasenia incorrectos');
                                 writeln('Por favor ingrese nuevamente los datos. Presione ENTER para continuar.');
                                 readln ();
                                 end;
                         	flag:= false;
                         end;
                    '2': begin
                         	flag:= true;
                         	bol:= false;
                         end;
               end;
            until ((flag) and (not bol));
          end;

Procedure GrabarArchivosBinarios (var AUSUARIOS: TarchUsuarios; var AMENSAJES: TarchMensajes);

          Var
          	Usuarios: text;
            RegUsuarios: TregUsuarios;

          begin
          	assign (Usuarios,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\USUARIO.txt');
            reset (Usuarios);
            rewrite (AUSUARIOS);
            while not eof (Usuarios) do
            	begin
               	readln (Usuarios,RegUsuarios.Id_Usuario);
                  readln (Usuarios,RegUsuarios.Password);
                  RegUsuarios.PosIni:= -1;
                  write (AUSUARIOS,RegUsuarios);
               end;
            close (Usuarios);
            close (AUSUARIOS);
            rewrite (AMENSAJES);
            close (AMENSAJES);
          end;

Procedure ListarArchivoUsuarios (var AUSUARIOS: TarchUsuarios; var Usuariostext: text);

          Var
          	RegUsuarios: TregUsuarios;

          begin
          	reset (AUSUARIOS);
            rewrite (Usuariostext);
            writeln (Usuariostext,'Lista por Usuarios');
            writeln (Usuariostext,'ID Usuario       ','Password       ','      Pos Ini');
            while not eof (AUSUARIOS) do
            	begin
               	read (AUSUARIOS,RegUsuarios);
                  write (Usuariostext,RegUsuarios.Id_Usuario:10);
                  write (Usuariostext,'      ');
                  write (Usuariostext,RegUsuarios.Password:10);
                  write (Usuariostext,'      ');
                  writeln (Usuariostext,RegUsuarios.PosIni:10);
               end;
            close (AUSUARIOS);
            close (Usuariostext);
          end;

Procedure ListarArchivoMensajes (var AMENSAJES: TarchMensajes; var Mensajestext: text);

          Var
             RegMensajes: TregMensajes;

          begin
               reset (AMENSAJES);
               rewrite (Mensajestext);
               writeln (Mensajestext,'  Fecha      ','Id_usuario     ','Destinatario       ','Asunto      ','Senial');
               while not eof (AMENSAJES) do
                     	begin
                        	read (AMENSAJES,RegMensajes);
                           write (Mensajestext,RegMensajes.Fecha,'      ');
                           write (Mensajestext,RegMensajes.Id_Usuario,'        ');
                           write (Mensajestext,RegMensajes.Destinatario,'        ');
                           write (Mensajestext,RegMensajes.Asunto,'     ');
                           writeln (Mensajestext,RegMensajes.Senial);
                        end;
               close (AMENSAJES);
               close (Mensajestext);
          end;

Procedure CrearArchivosBinarios;

          Var
             x: char;
             AUSUARIOS: TarchUsuarios;
             AMENSAJES: TarchMensajes;
             Usuariostext: text;
             Mensajestext: text;
             flag: boolean;
          	 begin
          		repeat
               clrscr;
               flag:= false;
               assign (AUSUARIOS,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AUSUARIOS.dat');
               assign (AMENSAJES,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\AMENSAJES.dat');
               assign (Usuariostext,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\Usuariostext.txt');
               assign (Mensajestext,'G:\Algoritmos y Estructura de Datos\Trabajo Práctico Cuatrimestral\Mensajestext.txt');
               writeln ('a. Grabar archivos binarios');
               writeln ('b. Listar archivo de usuarios');
               writeln ('c. Listar archivo de mensajes');
               writeln ('d. Volver al menu anterior');
               x:= readkey;
               case x of
                    'a': GrabarArchivosBinarios (AUSUARIOS,AMENSAJES);
                    'b': ListarArchivoUsuarios (AUSUARIOS,Usuariostext);
                    'c': ListarArchivoMensajes (AMENSAJES,Mensajestext);
                    'd': flag:= true
               end;
             until flag;
          end;

Procedure Aplicacion;

          Var
             x: char;
             url: string;
             flag: boolean;

          begin
          	repeat
               clrscr;
               flag:= false;
               writeln ('1. Crear archivo binario de usuarios y mensajes');
               writeln ('2. Acceder a Amail 2011');
               writeln ('3. Salir de la Aplicacion');
               x:= readkey;
               case x of
                    '1': CrearArchivosBinarios;
                    '2': begin
                         clrscr;
                         writeln ('Ingrese la direccion web a la que desea acceder');
                         readln (url);
                         if url= 'www.amail2011.com'
                            then Menulogueo
                            else begin
                                      repeat
                                      clrscr;
                                      writeln ('Error: URL incorrecta.');
                                      writeln ('Ingrese nuevamente la direccion web a la que desea acceder');
                                      readln (url);
                                      until url= 'www.amail2011.com';
                                      Menulogueo;
                                 end;
                         end;
                    '3': flag:= true;
               end;
               until flag;
          end;

Begin
	clrscr;
	Aplicacion;
end.