package martin.tempest.sources;

import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JLabel;
import javax.swing.JTextField;

public class TSDRLimeSDRSource extends TSDRSource {

    public TSDRLimeSDRSource() {
        super("LimeSDR Mini (live)", "TSDRPlugin_LimeSDR", false);
    }

    @Override
    public boolean populateGUI(final Container cont, final String defaultprefs, final ActionListenerRegistrator okbutton) {
        // Valeurs par défaut
        String defaultFrequency = "100000000"; // 100 MHz
        String defaultSamplerate = "8000000";  // 8 MS/s

        // Si des préférences existent, les appliquer
        if (defaultprefs != null && !defaultprefs.isEmpty()) {
            String[] parts = defaultprefs.split(" ");
            if (parts.length >= 2) {
                defaultFrequency = parts[0];
                defaultSamplerate = parts[1];
            }
        }

        final JLabel labelFreq = new JLabel("Fréquence (Hz) :");
        labelFreq.setBounds(12, 12, 150, 24);
        cont.add(labelFreq);

        final JTextField freqField = new JTextField(defaultFrequency);
        freqField.setBounds(160, 12, 200, 24);
        cont.add(freqField);

        final JLabel labelRate = new JLabel("Taux échantillonnage (S/s) :");
        labelRate.setBounds(12, 50, 200, 24);
        cont.add(labelRate);

        final JTextField rateField = new JTextField(defaultSamplerate);
        rateField.setBounds(220, 50, 140, 24);
        cont.add(rateField);

        okbutton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String params = freqField.getText().trim() + " " + rateField.getText().trim();
                setParams(params);
            }
        });

        return true;
    }
}

