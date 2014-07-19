package org.ledyba.functional;

public interface Func<I, R> {
	R apply(final I i);
}
