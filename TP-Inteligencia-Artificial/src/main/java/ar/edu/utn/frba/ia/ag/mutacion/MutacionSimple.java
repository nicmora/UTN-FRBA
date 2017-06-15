package main.java.ar.edu.utn.frba.ia.ag.mutacion;

import java.util.List;

import main.java.ar.edu.utn.frba.ia.ag.Estado;
import main.java.ar.edu.utn.frba.ia.ag.Individuo;

public class MutacionSimple extends Mutacion {
	
	private double probabilidadConstante;
	
	public MutacionSimple(double probabilidadConstante) {
		this.probabilidadConstante = probabilidadConstante;
	}
	
	@Override
	protected double getProbabilidadDeMutacion(List<Individuo> individuos, Estado estado) {
		return probabilidadConstante;
	}
	
}
