package org.ledyba.noiz2sa.donation;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.ledyba.functional.Either;
import org.ledyba.functional.Func;
import org.ledyba.functional.Left;
import org.ledyba.functional.Right;
import org.ledyba.noiz2sa.R;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender.SendIntentException;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.android.vending.billing.IInAppBillingService;

public class DonationActivity extends Activity {
	private final ExecutorService th = Executors.newCachedThreadPool();
	private Handler hd = new Handler();
	private BillingWrapper billing_ = null;
	private ServiceConnection serviceConnection_ = null;
	
	private static final List<String> ProductIDs = Arrays.asList("one", "five", "ten");
	
	private final static String TAG="DonationActivity";

	public DonationActivity() {
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_donation);
		ListView lv = (ListView) findViewById(R.id.products);
		lv.setAdapter(this.adapter = new DonationAdapter());
	}
	
	private DonationAdapter adapter;
	
	private class DonationAdapter extends BaseAdapter {
		private List<Product> products;
		private List<Purchase> purchases;
		public void update(final List<Product> products, final List<Purchase> purchases){
			this.products = products;
			this.purchases = purchases;
			this.notifyDataSetInvalidated();
		}

		@Override
		public int getCount() {
			return products == null ? 0 : this.products.size();
		}

		@Override
		public Object getItem(int position) {
			return products.get(position);
		}

		@Override
		public long getItemId(int position) {
			return 0;
		}
		
		private Purchase getPurchaseOf(final String prodId){
			for(Purchase p : purchases){
				if(p.getProductId().equals(prodId) && p.getPurchaseState() == Purchase.State.Purchased){
					return p;
				}
			}
			return null;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			ViewGroup v = (ViewGroup)(convertView == null ? LayoutInflater.from(DonationActivity.this).inflate(R.layout.item_donation, null) : convertView);
			TextView title = (TextView) v.findViewById(R.id.title);
			TextView desc = (TextView) v.findViewById(R.id.description);
			TextView price = (TextView) v.findViewById(R.id.price);
			TextView priceText = (TextView) v.findViewById(R.id.priceText);
			final Product p = products.get(position);
			final Purchase pu = getPurchaseOf( p.getProductId() );
			title.setText(p.getTitle());
			desc.setText(p.getDescription());
			price.setText(p.getPrice());
			v.setEnabled(true);
			if(pu != null){
				title.setEnabled(false);
				desc.setEnabled(false);
				price.setEnabled(false);
				priceText.setEnabled(false);
				v.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						Toast.makeText(DonationActivity.this, "Already donated.", Toast.LENGTH_SHORT).show();
					}
				});
			}else{
				title.setEnabled(true);
				desc.setEnabled(true);
				price.setEnabled(true);
				priceText.setEnabled(true);
				v.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						startBuy(p);
					}
				});
			}
			return v;
		}
	}
	
	private void startBuy(final Product p){
		billing_.createIntentFor(p, "").bind(new Func<PendingIntent, Either<Exception, Void>>() {
			@Override
			public Either<Exception, Void> apply(PendingIntent pi) {
				try {
					startIntentSenderForResult(pi.getIntentSender(), 0, new Intent(), 0, 0, 0);
					return new Right<Exception, Void>(null);
				} catch (SendIntentException e) {
					return new Left<Exception, Void>(e);
				}
			}
		}).ifLeft(new Func<Exception, Void>() {
			@Override
			public Void apply(Exception i) {
				Log.e(TAG, "Error on start buy intent: ", i);
				return null;
			}
		});
	}
	
	private void listUp(){
		th.submit(new Runnable() {
			@Override
			public void run() {
				billing_.fetchProductDetails(ProductIDs).bind(new Func<List<Product>, Either<Exception,Void>>() {
					@Override
					public Either<Exception, Void> apply(final List<Product> products) {
					return billing_.fetchPurchases().bind(new Func<List<Purchase>, Either<Exception,Void>>() {
						@Override
						public Either<Exception, Void> apply(final List<Purchase> purchases) {
							hd.post(new Runnable() {
								@Override
								public void run() {
									updateItems(products, purchases);
								}
							});
							return null;
						}
					});
					}
				}).ifLeft(new Func<Exception, Void>() {

					@Override
					public Void apply(Exception i) {
						Log.e(TAG, "Exception on updating catalog: ",i);
						findViewById(R.id.now_loading).setVisibility(View.GONE);
						findViewById(R.id.error).setVisibility(View.VISIBLE);
						return null;
					}
				});
			}
		});
	}
	private void updateItems(final List<Product> products, final List<Purchase> purchases){
		findViewById(R.id.now_loading).setVisibility(View.GONE);
		if( products.size() > 0 ) {
			findViewById(R.id.products).setVisibility(View.VISIBLE);
			this.adapter.update(products, purchases);
		}else{
			findViewById(R.id.not_found).setVisibility(View.VISIBLE);
		}
	}
	@Override
	protected void onResume() {
		super.onResume();

		this.serviceConnection_ = new ServiceConnection() {
			@Override
			public void onServiceConnected(ComponentName name, IBinder service) {
				Log.d(TAG, "ServiceConnected: "+name);
				billing_ = new BillingWrapper(DonationActivity.this, IInAppBillingService.Stub.asInterface(service));
				listUp();
			}

			@Override
			public void onServiceDisconnected(ComponentName name) {
				Log.d(TAG, "ServiceDisconnected: "+name);
				billing_ = null;
			}
		};

		// バインドする
		this.bindService(new Intent("com.android.vending.billing.InAppBillingService.BIND"), this.serviceConnection_, Context.BIND_AUTO_CREATE);
		Log.d(TAG, "Bind service: InAppBillingService");
	}

	@Override
	protected void onPause() {
		if (this.serviceConnection_ != null) {
			unbindService(this.serviceConnection_);
			Log.d(TAG, "Unbind service: InAppBillingService");
			this.serviceConnection_ = null;
		}
		super.onPause();
	}
	@Override
	protected void onDestroy() {
		//startActivity(new Intent(this, org.ledyba.noiz2sa.Noiz2saActivity.class));     
		super.onDestroy();
	}

}
