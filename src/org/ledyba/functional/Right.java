package org.ledyba.functional;

public class Right<E, A> extends Either<E, A> {

	private final A spirit;
	public Right(A answer) {
		this.spirit = answer;
	}
	@Override
	public boolean isLeft() {
		return false;
	}
	@Override
	public boolean isRight() {
		return true;
	}
	@Override
	public E getLeftOr(E default_) {
		return default_;
	}
	@Override
	public E getLeft() {
		throw new IllegalStateException("You cannot get left value from right.");
	}
	@Override
	public A getRightOr(A default_) {
		return spirit;
	}
	@Override
	public A getRight() {
		return spirit;
	}
	@Override
	public Either<E, A> ifRight(Func<A, ?> fun) {
		fun.apply(spirit);
		return this;
	}
	@Override
	public Either<E, A> ifLeft(Func<E, ?> fun) {
		return this;
	}

}
