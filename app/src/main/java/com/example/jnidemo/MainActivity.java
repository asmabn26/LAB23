package com.example.jnidemo;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // Méthodes natives
    public native String getNativeMessage();
    public native int computeProduct(int n);
    public native String flipText(String txt);
    public native int calculateSum(int[] array);

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView txtHello = findViewById(R.id.tvHello);
        TextView txtFact = findViewById(R.id.tvFact);
        TextView txtReverse = findViewById(R.id.tvReverse);
        TextView txtArrayData = findViewById(R.id.tvArrayData);
        TextView txtArray = findViewById(R.id.tvArray);

        // 1. Message natif
        txtHello.setText(getNativeMessage());

        // 2. Factoriel
        int number = 5;
        int factResult = computeProduct(number);
        if (factResult >= 0) {
            txtFact.setText(number + "! = " + factResult);
        } else if (factResult == -1) {
            txtFact.setText("Erreur : valeur négative");
        } else {
            txtFact.setText("Erreur : dépassement");
        }

        // 3. Inversion de texte
        String originalText = "Asma Bensassi Nour";
        String reversedText = flipText(originalText);
        txtReverse.setText(
            "Original : " + originalText +
            "\nInversé  : " + reversedText
        );

        // 4. Somme tableau
        int[] values = {53, 104, 150, 200};
        int sumResult = calculateSum(values);
        
        // Affichage des données d'entrée en petite taille en haut
        txtArrayData.setText("Entrée: " + Arrays.toString(values));
        
        if (sumResult >= 0) {
            // Affichage de la somme en grand au centre
            txtArray.setText("Somme = " + sumResult);
        } else {
            txtArray.setText("Erreur (code " + sumResult + ")");
        }
    }
}