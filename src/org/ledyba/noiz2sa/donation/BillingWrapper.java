package org.ledyba.noiz2sa.donation;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;
import org.ledyba.functional.Either;
import org.ledyba.functional.Func;
import org.ledyba.functional.Left;
import org.ledyba.functional.Right;

import android.app.PendingIntent;
import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;

import com.android.vending.billing.IInAppBillingService;

public class BillingWrapper {
	private final static int BillingVersion = 3;
	private final String packageName;
	private final IInAppBillingService spirit;

	public BillingWrapper(Context ctx, IInAppBillingService spirit) {
		this.packageName = ctx.getPackageName();
		this.spirit = spirit;
	}

	public Either<Exception, Boolean> isBillingSupported(){
		try {
			final int supported = this.spirit.isBillingSupported(BillingVersion, packageName, "inapp");
			return new Right<Exception, Boolean>( supported == 0 );
		} catch (RemoteException e) {
			return new Left<Exception, Boolean>(e);
		}
	}
	
	public Either<Exception, List<Product> > fetchProductDetails( final List<String> ids ){
		final Bundle bun = new Bundle();
		bun.putStringArrayList("ITEM_ID_LIST", new ArrayList<String>(ids));
		List<Product> r = new ArrayList<Product>();
		try {
			final Bundle res = this.spirit.getSkuDetails(BillingVersion, packageName, "inapp", bun);
			final int respCode = res.getInt("RESPONSE_CODE");
			if( respCode != 0 ) { //error
				new Left<Exception, List<Product>>(new Exception(errorToString(respCode)));
			}
			final List<String> details = res.getStringArrayList("DETAILS_LIST");
			for( String detail : details ){
				r.add(Product.fromString(detail));
			}
			return new Right<Exception, List<Product>>(r);
		} catch (RemoteException e) {
			return new Left<Exception, List<Product>>(e);
		} catch (JSONException e) {
			return new Left<Exception, List<Product>>(e);
		}
	}
	
	public Either<Exception, List<Purchase>> fetchPurchases(){
		final List<String> itemList = new ArrayList<String>();
		final List<String> dataList = new ArrayList<String>();
		final List<String> signatureList = new ArrayList<String>();
		
		final Func<String, Either<Exception, String> > fetch = new Func<String, Either<Exception,String>>() {
			@Override
			public Either<Exception, String> apply(String i) {
				Bundle b;
				try {
					b = spirit.getPurchases(BillingVersion, packageName, "inapp", null);
				} catch (RemoteException e) {
					return new Left<Exception, String>(e);
				}
				final int respCode_ = b.getInt("RESPONSE_CODE");
				if( respCode_ != 0 ) {
					return new Left<Exception, String>(new Exception(errorToString(respCode_)));
				}
				itemList.addAll(b.getStringArrayList("INAPP_PURCHASE_ITEM_LIST"));
				dataList.addAll( b.getStringArrayList("INAPP_PURCHASE_DATA_LIST") );
				signatureList.addAll( b.getStringArrayList("INAPP_DATA_SIGNATURE_LIST"));
				final String tok = b.getString("INAPP_CONTINUATION_TOKEN");
				return tok == null ? new Right<Exception, String>("OK") : this.apply(tok);
			}
		};
		
		return fetch.apply(null).bind(new Func<String, Either<Exception, List<Purchase>>>() {
			@Override
			public Either<Exception, List<Purchase>> apply(String i) {
				List<Purchase> r = new ArrayList<Purchase>();
				for( String data : dataList ){
					try {
						r.add(Purchase.fromString(data));
					} catch (JSONException e) {
						return new Left<Exception, List<Purchase>>(e);
					}
				}
				return new Right<Exception, List<Purchase>>(r);
			}
		});
	}
	public Either<Exception, Boolean> consumePurchase(final Purchase p){
		try {
			final int code = this.spirit.consumePurchase(BillingVersion, packageName, p.getPurchaseToken());
			if ( 0 == code ){
				return new Right<Exception, Boolean>(true);
			}else{
				return new Left<Exception, Boolean>(new IllegalStateException(errorToString(code)));
			}
		} catch (RemoteException e) {
			return new Left<Exception, Boolean>(e);
		}
	}
	private String errorToString(int err){
		switch (err) {
		case 0: //RESULT_OK = 0 - success
			return "OK";
		case 1: //RESULT_USER_CANCELED = 1 - user pressed back or canceled a dialog
			return "User canceled.";
		case 3: //RESULT_BILLING_UNAVAILABLE = 3 - this billing API version is not supported for the type requested
			return "Billing unavailable";
		case 4: //RESULT_ITEM_UNAVAILABLE = 4 - requested SKU is not available for purchase
			return "Item unavailable";
		case 5: //RESULT_DEVELOPER_ERROR = 5 - invalid arguments provided to the API
			return "Developer error";
		case 6: //RESULT_ERROR = 6 - Fatal error during the API action
			return "Fatal error";
		case 7: //RESULT_ITEM_ALREADY_OWNED = 7 - Failure to purchase since item is already owned
			return "Item already owned";
		case 8: //RESULT_ITEM_NOT_OWNED = 8 - Failure to consume since item is not owned
			return "Item not found";
		default:
			return "????";
		}
	}
	
	public Either<Exception, PendingIntent> createIntentFor(Product prod, String payload){
		Bundle b;
		try {
			b = this.spirit.getBuyIntent(BillingVersion, packageName, prod.getProductId(), "inapp", payload);
			final int respCode_ = b.getInt("RESPONSE_CODE");
			if( respCode_ != 0 ) {
				return new Left<Exception, PendingIntent>(new Exception(errorToString(respCode_)));
			}
			return new Right<Exception, PendingIntent>( b.<PendingIntent>getParcelable("BUY_INTENT") );
		} catch (RemoteException e) {
			return new Left<Exception, PendingIntent>(e);
		}
	}
	
}

final class Purchase {
	private final String orderId_;
	private final String packageName_;
	private final String productId_;
	private final Date purchaseTime_;
	public enum State{
		Purchased, Canceled, Refunded
	};
	private final State purchaseState_;
	private final String developerPayload_;
	private final String purchaseToken_;
	private Purchase(
			final String orderId,
			final String packageName,
			final String productId,
			final Date   purchaseTime,
			final State  purchaseState,
			final String developerPayload,
			final String purchaseToken
	){
		this.orderId_          = orderId;
		this.packageName_      = packageName;
		this.productId_        = productId;
		this.purchaseTime_     = purchaseTime;
		this.purchaseState_    = purchaseState;
		this.developerPayload_ = developerPayload;
		this.purchaseToken_    = purchaseToken;
	}
	public static Purchase fromString( final String detail ) throws JSONException{
		JSONObject object = new JSONObject( detail );
		final String orderId = object.getString("orderId");
		final String packageName = object.getString("packageName");
		final String productId = object.getString("productId");
		final Date purchaseTime = new Date(object.getLong("purchaseTime"));
		final State purchaseState = State.values()[object.getInt("purchaseState")];
		final String developerPayload = object.getString("developerPayload");
		final String purchaseToken = object.getString("purchaseToken");
		return new Purchase(
				orderId,
				packageName,
				productId,
				purchaseTime,
				purchaseState,
				developerPayload,
				purchaseToken
		);
	}
	public String getOrderId() {
		return orderId_;
	}
	public String getPackageName() {
		return packageName_;
	}
	public String getProductId() {
		return productId_;
	}
	public Date getPurchaseTime() {
		return purchaseTime_;
	}
	public State getPurchaseState() {
		return purchaseState_;
	}
	public String getDeveloperPayload() {
		return developerPayload_;
	}
	public String getPurchaseToken() {
		return purchaseToken_;
	}

}

final class Product {
	private final String productId_;
	private final String title_;
	private final String description_;
	private final String price_;
	private final String type_;
	public Product(final String productId, final String type, final String price, final String title, final String desc){
		this.productId_ = productId;
		this.type_ = type;
		this.price_ = price;
		this.title_ = title;
		this.description_ = desc;
	}
	public static Product fromString( final String detail ) throws JSONException{
		JSONObject object = new JSONObject( detail );
		final String productId = object.getString("productId");
		final String type = object.getString("type");
		final String price = object.getString("price");
		final String title = object.getString("title");
		final String description = object.getString("description");
		return new Product(productId, type, price, title, description);
	}
	public String getProductId() {
		return productId_;
	}
	public String getTitle() {
		return title_;
	}
	public String getDescription() {
		return description_;
	}
	public String getPrice() {
		return price_;
	}
	public String getType() {
		return type_;
	}
	
	
}
