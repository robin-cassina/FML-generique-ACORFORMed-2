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
package greta.tools.editors;

import java.io.File;

/**
 *
 * @author Andre-Marie
 */
public class XMLFileChooser extends javax.swing.filechooser.FileFilter{

    @Override
    public boolean accept(File f) {
        return f.getName().endsWith(".xml") || f.isDirectory();
    }

    @Override
    public String getDescription() {
        return "XML files";
    }

}
