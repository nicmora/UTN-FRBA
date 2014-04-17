package comprarEntradas.ui;

import java.awt.Color;

import org.uqbar.arena.actions.MessageSend;
import org.uqbar.arena.layout.ColumnLayout;
import org.uqbar.arena.layout.HorizontalLayout;
import org.uqbar.arena.widgets.Button;
import org.uqbar.arena.widgets.Label;
import org.uqbar.arena.widgets.Panel;
import org.uqbar.arena.widgets.TextBox;
import org.uqbar.arena.widgets.tables.Column;
import org.uqbar.arena.widgets.tables.Table;
import org.uqbar.arena.windows.SimpleWindow;
import org.uqbar.arena.windows.WindowOwner;

import comprarEntradas.domain.Banda;


@SuppressWarnings("serial")
public class BuscarBandasNombreFestivalWindow extends SimpleWindow<BuscadorBandas>{

	public BuscarBandasNombreFestivalWindow(WindowOwner owner) {
		super(owner, new BuscadorBandas());
		this.getModelObject().searchBandas();
	}
	
	@Override
	protected void createMainTemplate(Panel mainPanel) {
		this.setTitle("Festival de Rock");
		this.setTaskDescription("Ingrese los parametros de busqueda");

		super.createMainTemplate(mainPanel);

		this.createResultsGrid(mainPanel);
		this.createGridActions(mainPanel);
	}
	
	@Override
	protected void createFormPanel(Panel mainPanel) {
		Panel searchFormPanel = new Panel(mainPanel);
		searchFormPanel.setLayout(new ColumnLayout(2));
		
		new Label(searchFormPanel).setText("Banda").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("nombreBanda");
		
		new Label(searchFormPanel).setText("Festival").setForeground(Color.BLUE);
		new TextBox(searchFormPanel).setWidth(50).bindValueToProperty("festivalID");
	}

	@Override
	protected void addActions(Panel actionsPanel) {
		new Button(actionsPanel)
			.setCaption("Buscar")
			.onClick(new MessageSend(this.getModelObject(), "searchBandas"))
			.setAsDefault()
			.disableOnError();

		new Button(actionsPanel)
			.setCaption("Limpiar")
			.onClick(new MessageSend(this.getModelObject(), "clear"));
	}
	
	protected void createResultsGrid(Panel mainPanel) {
		Table<Banda> table = new Table<Banda>(mainPanel, Banda.class);
		table.setHeigth(250);
		table.setWidth(400);
		table.bindItemsToProperty("resultados");

		this.describeResultsGrid(table);
	}
	
	protected void describeResultsGrid(Table<Banda> table) {		
		new Column<Banda>(table)
			.setTitle("Banda")
			.setFixedSize(80)
			.bindContentsToProperty("nombre");
		
		new Column<Banda>(table)
		.setTitle("Categoria")
		.setFixedSize(80)
		.bindContentsToProperty("nroCategoria");
	}
	
	protected void createGridActions(Panel mainPanel) {
		Panel actionsPanel = new Panel(mainPanel);
		actionsPanel.setLayout(new HorizontalLayout());
	}
}