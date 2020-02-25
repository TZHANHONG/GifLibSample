package com.example.gifndk;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.ImageView;

import com.bumptech.glide.Glide;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private ImageView imageView;
    private String path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator +
            "gif.gif";
    private Bitmap bitmap;
    private int maxLength;
    private int currentLength;
    private GifHandler gifHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView = findViewById(R.id.image);
        imageView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                load();
            }
        });

    }

    private void load() {
        gifHandler = new GifHandler(path);
        int width = gifHandler.getWidth();
        int height = gifHandler.getHeight();
        bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        maxLength = gifHandler.getLength();

        long delayTime = gifHandler.renderFrame(bitmap, currentLength);
        imageView.setImageBitmap(bitmap);
        if (handler != null) {
            handler.sendEmptyMessageDelayed(1, delayTime);
        }
        /*Glide.with(this).asGif().load(path).into(imageView);*/
    }

    Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message msg) {
            currentLength++;
            if (currentLength >= maxLength) {
                currentLength = 0;
            }
            long delayTime = gifHandler.renderFrame(bitmap, currentLength);
            imageView.setImageBitmap(bitmap);
            handler.sendEmptyMessageDelayed(0, delayTime);
            return false;
        }
    });
}
