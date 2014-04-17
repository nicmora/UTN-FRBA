package comprarEntradas.ui;

import org.uqbar.arena.Application;
import org.uqbar.arena.windows.Window;


public class FestivalDeRockApplication extends Application {
	
	public static void main(String[] args) {
		new FestivalDeRockApplication().start();
	}

	@Override
	protected Window<?> createMainWindow() {
		return new MenuPrincipalWindow(this);
	}	
}
