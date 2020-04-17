package com.kevin.kevinplayer;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // 下面都是媒体流资源
//    private final static String PATH = Environment.getExternalStorageDirectory() + File.separator + "demo.mp4";
     private final static String PATH = "rtmp://58.200.131.2:1935/livetv/hunantv";

    private KevinPlayer kevinPlayer;

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surface_view);

        kevinPlayer = new KevinPlayer();

        kevinPlayer.setSurfaceView(surfaceView);

        Toast.makeText(this, "FFmpeg" + kevinPlayer.getFFmpegVersion(), Toast.LENGTH_SHORT).show();

        kevinPlayer.setDataSource(PATH);
        kevinPlayer.setOnPreparedListener(new KevinPlayer.OnPreparedListener() {

            // 准备成功
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("UI")
                                .setMessage("准备好了，开始播放 ...")
                                .setPositiveButton("老夫知道了", null)
                                .show();
                    }
                });
                // 准备成功之后，开始播放 视频 音频
                kevinPlayer.start();
            }

            // 准备失败
            @Override
            public void onError(final String errorText) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("Error")
                                .setMessage("已经发生错误，请查阅:" + errorText)
                                .setPositiveButton("我来个去，什么情况", null)
                                .show();
                    }
                });
            }
        });


    }

    @Override
    protected void onResume() {
        super.onResume();
        kevinPlayer.prepare();
    }

    @Override
    protected void onPause() {
        super.onPause();
        kevinPlayer.stop();
    }

    /*@Override
    protected void onStop() {
        super.onStop();
        kevinPlayer.stop();
    }*/

    @Override
    protected void onDestroy() {
        super.onDestroy();
        kevinPlayer.release();
    }
}
