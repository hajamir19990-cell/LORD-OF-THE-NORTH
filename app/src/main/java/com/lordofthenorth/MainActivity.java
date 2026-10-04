package com.lordofthenorth;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Color;
import android.view.Gravity;
import android.widget.TextView;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        TextView screen = new TextView(this);
        screen.setText("LORD OF THE NORTH");
        screen.setTextColor(Color.WHITE);
        screen.setTextSize(28);
        screen.setGravity(Gravity.CENTER);
        screen.setBackgroundColor(Color.rgb(8, 17, 23));

        setContentView(screen);
    }
}
