/*
 * This file is part of Greta.
 *
 * Greta is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Greta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Greta.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
package greta.tools.animation.gestureeditor;

import greta.core.signals.gesture.Hand;

/**
 *
 * @author Jing Huang
 */
public class UniformOneHandPanel extends javax.swing.JPanel {

    /**
     * Creates new form UniformPhasePanel
     */
    public UniformOneHandPanel() {
        initComponents();
    }

    public HandOrientationEulerPanel getHandOrientationPanel() {
        return handOrientationPanel;
    }

    public HandShapePanel getHandShapePanel() {
        return handShapePanel;
    }

    public UniformHandPositionPanel getUniformHandPositionPanel() {
        return uniformHandPositionPanel;
    }

    public void loadHand(Hand hand, GestureEditor parent) {
        if (hand == null) {
            return;
        }
        uniformHandPositionPanel.loadHand(hand, parent);
        handOrientationPanel.loadHand(hand, parent);
        handShapePanel.loadHand(hand, parent);
        opennessPanel.loadHand(hand, parent);
        trajectoryPanel.loadHand(hand, parent);
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        spatialityLabel = new javax.swing.JLabel();
        spatialityArrowLabel = new javax.swing.JLabel();
        ideationalUnitsLabel = new javax.swing.JLabel();
        ideationalUnitsArrowLabel = new javax.swing.JLabel();
        uniformHandPositionPanel = new greta.tools.animation.gestureeditor.UniformHandPositionPanel();
        handOrientationPanel = new greta.tools.animation.gestureeditor.HandOrientationEulerPanel();
        handShapePanel = new greta.tools.animation.gestureeditor.HandShapePanel();
        opennessPanel = new greta.tools.animation.gestureeditor.OpennessPanel();
        trajectoryPanel = new greta.tools.animation.gestureeditor.TrajectoryPanel();

        setBorder(javax.swing.BorderFactory.createTitledBorder("Uniform"));

        spatialityLabel.setText("Spatiality:");

        spatialityArrowLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        spatialityArrowLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        spatialityArrowLabel.setText("<html>&#x21E3;</html>");

        ideationalUnitsLabel.setText("Ideational Units:");

        ideationalUnitsArrowLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        ideationalUnitsArrowLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        ideationalUnitsArrowLabel.setText("<html>&#x21E3;</html>");

        trajectoryPanel.setBorder(javax.swing.BorderFactory.createTitledBorder("Trajectory"));

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(uniformHandPositionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(handOrientationPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
                            .addComponent(handShapePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(opennessPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(trajectoryPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(spatialityLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(spatialityArrowLabel))
                        .addGap(18, 18, 18)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(ideationalUnitsLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(ideationalUnitsArrowLabel))))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(spatialityLabel)
                    .addComponent(ideationalUnitsLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(ideationalUnitsArrowLabel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(spatialityArrowLabel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(0, 0, 0)
                .addComponent(uniformHandPositionPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(handOrientationPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(handShapePanel, javax.swing.GroupLayout.PREFERRED_SIZE, 69, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(opennessPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(trajectoryPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private greta.tools.animation.gestureeditor.HandOrientationEulerPanel handOrientationPanel;
    private greta.tools.animation.gestureeditor.HandShapePanel handShapePanel;
    private javax.swing.JLabel ideationalUnitsArrowLabel;
    private javax.swing.JLabel ideationalUnitsLabel;
    private greta.tools.animation.gestureeditor.OpennessPanel opennessPanel;
    private javax.swing.JLabel spatialityArrowLabel;
    private javax.swing.JLabel spatialityLabel;
    private greta.tools.animation.gestureeditor.TrajectoryPanel trajectoryPanel;
    private greta.tools.animation.gestureeditor.UniformHandPositionPanel uniformHandPositionPanel;
    // End of variables declaration//GEN-END:variables
}
