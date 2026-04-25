package com.example.jnidemo;

import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // 🔥 Détection de Debugger / Menace
    public native boolean isDebugDetected();

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

        TextView txtStatus = findViewById(R.id.tvStatus);
        TextView txtHello = findViewById(R.id.tvHello);
        TextView txtFact = findViewById(R.id.tvFact);
        TextView txtReverse = findViewById(R.id.tvReverse);
        TextView txtArrayData = findViewById(R.id.tvArrayData);
        TextView txtArray = findViewById(R.id.tvArray);

        // 🔍 Exécution du contrôle de sécurité natif
        boolean danger = isDebugDetected();

        if (danger) {
            // Statut visuel d'alerte
            txtStatus.setText("⚠️ ENVIRONNEMENT SUSPECT DÉTECTÉ");
            txtStatus.setTextColor(Color.RED);

            // Désactivation des fonctions (Blocage logique)
            txtHello.setText("Accès refusé");
            txtFact.setText("BLOQUÉ");
            txtReverse.setText("Fonction sécurisée désactivée");
            txtArrayData.setText("—");
            txtArray.setText("🔒");
            
            return; // Arrêt de l'exécution pour protéger les parties sensibles
        } else {
            // Statut visuel sécurisé
            txtStatus.setText("✔️ ENVIRONNEMENT SÉCURISÉ");
            txtStatus.setTextColor(Color.parseColor("#2E7D32"));
            
            // Exécution normale des fonctions natives
            txtHello.setText(getNativeMessage());

            int number = 5;
            int factResult = computeProduct(number);
            if (factResult >= 0) {
                txtFact.setText(number + "! = " + factResult);
            } else {
                txtFact.setText("Erreur calcul");
            }

            String originalText = "Asma Bensassi Nour";
            txtReverse.setText("Original : " + originalText + "\nInversé  : " + flipText(originalText));

            int[] values = {53, 104, 150, 200};
            txtArrayData.setText("Entrée: " + Arrays.toString(values));
            txtArray.setText("Somme = " + calculateSum(values));
        }
    }
}