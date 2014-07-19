package org.ledyba.functional;

public abstract class Either<E, A> {
	protected Either() {
	}
	
	public abstract boolean isLeft();
	public abstract boolean isRight();
	
	public abstract E getLeftOr(E default_);
	public abstract E getLeft();

	public abstract A getRightOr(A default_);
	public abstract A getRight();
	
	public abstract Either<E,A> ifRight(Func<A,?> fun);
	public abstract Either<E,A> ifLeft(Func<E,?> fun);
	
	static public final <E,A> Left<E,A> left(E e){
		return new Left<E,A>(e);
	}
	static public final <E,A> Right<E,A> right(A a){
		return new Right<E,A>(a);
	}
	
	public <A2> Either<E,A2> bind( Func<A, Either<E,A2> > f ) {
		if(isLeft()) {
			@SuppressWarnings("unchecked") Either<E,A2> r = (Either<E,A2>)this;
			return r;
		}else{
			return f.apply(getRight());
		}
	}
	public <A2> Either<E,A2> fmap( Func<A, A2> f ) {
		if(isLeft()) {
			@SuppressWarnings("unchecked") Either<E,A2> r = (Either<E,A2>)this;
			return r;
		}else{
			return new Right<E,A2>(f.apply(getRight()));
		}
	}
}
