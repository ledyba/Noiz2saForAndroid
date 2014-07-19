package org.ledyba.functional;

public class Left<E, A> extends Either<E, A> {
	private final E spirit;
	public Left(E error) {
		this.spirit = error;
	}
	@Override
	public boolean isLeft() {
		return true;
	}
	@Override
	public boolean isRight() {
		return false;
	}
	@Override
	public E getLeftOr(E default_) {
		return spirit;
	}
	@Override
	public E getLeft() {
		return spirit;
	}
	@Override
	public A getRightOr(A default_) {
		return default_;
	}
	@Override
	public A getRight() {
		throw new IllegalStateException("You cannot get right value from left.");
	}
	@Override
	public Either<E, A> ifRight(Func<A, ?> fun) {
		return this;
	}
	@Override
	public Either<E, A> ifLeft(Func<E, ?> fun) {
		fun.apply(spirit);
		return this;
	}

}
