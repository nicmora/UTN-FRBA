package comprarEntradas.ui;

import java.awt.Color;

import org.uqbar.arena.actions.MessageSend;
import org.uqbar.arena.layout.ColumnLayout;
import org.uqbar.arena.widgets.Button;
import org.uqbar.arena.widgets.Label;
import org.uqbar.arena.widgets.Panel;
import org.uqbar.arena.widgets.TextBox;
import org.uqbar.arena.widgets.tables.Column;
import org.uqbar.arena.widgets.tables.Table;
import org.uqbar.arena.windows.WindowOwner;

import comprarEntradas.domain.Entrada;


@SuppressWarnings("serial")
public class BuscarEntradasPuestoDeVentaFestivalWindow extends VentaDeEntradasWindow{

	public BuscarEntradasPuestoDeVentaFestivalWindow(WindowOwner owner) {
		super(owner);
		this.getModelObject().searchOcupadas();
	}
	
	//El panel principal de búsqueda permite filtrar por nombre del cliente, apellido del cliente, fechas desde y hasta.
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		Panel searchFormPanel = new Panel(mainPanel);
		searchFormPanel.setLayout(new ColumnLayout(2));
		
		new Label(searchFormPanel).setText("Puesto de Venta").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("numeroPuestoDeVenta");
		
		new Label(searchFormPanel).setText("Festival").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("festivalID");
		
	}
	
	//Acciones asociadas a la pantalla.
	
		@Override
		protected void addActions(Panel actionsPanel) {
			new Button(actionsPanel)
				.setCaption("Buscar")
				.onClick(new MessageSend(this.getModelObject(), "searchOcupadas"))
				.setAsDefault()
				.disableOnError();

			new Button(actionsPanel) //
				.setCaption("Limpiar")
				.onClick(new MessageSend(this.getModelObject(), "clear"));
		}
		
		// *************************************************************************
		// ** RESULTADOS DE LA BUSQUEDA
		// *************************************************************************

		/*
		 * Se crea la grilla en el panel de abajo. El binding es: el contenido de la grilla en base a los
		 * resultados de la búsqueda. Cuando el usuario presiona Buscar, se actualiza el model, y éste a su vez
		 * dispara la notificación a la grilla que funciona como Observer.
		 */
		
		protected void createResultsGrid(Panel mainPanel) {
			Table<Entrada> table = new Table<Entrada>(mainPanel, Entrada.class);
			table.setHeigth(250);
			table.setWidth(550);
			table.bindItemsToProperty("resultados");

			this.describeResultsGrid(table);
		}
		
		//Define las columnas de la grilla.
		
		protected void describeResultsGrid(Table<Entrada> table) {
			new Column<Entrada>(table)
			.setTitle("Noche")
			.setFixedSize(80)
			.bindContentsToProperty("nroNoche");
			
			new Column<Entrada>(table)
			.setTitle("Sector")
			.setFixedSize(80)
			.bindContentsToProperty("sector");
			
			new Column<Entrada>(table)
			.setTitle("Fila")
			.setFixedSize(80)
			.bindContentsToProperty("fila");
			
			new Column<Entrada>(table)
			.setTitle("Butaca")
			.setFixedSize(80)
			.bindContentsToProperty("butaca");
		
			new Column<Entrada>(table)
			.setTitle("Precio")
			.setFixedSize(80)
			.bindContentsToProperty("precio");
			
			new Column<Entrada>(table)
			.setTitle("Puesto de Venta")
			.setFixedSize(80)
			.bindContentsToProperty("numeroPuestoDeVenta");
			
		}
		
		@Override
		protected void createGridActions(Panel mainPanel) {
			
		}
}