namespace ThreeD_SceneBuilder
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.loadSceneDialog = new System.Windows.Forms.OpenFileDialog();
            this.sceneTree = new System.Windows.Forms.TreeView();
            this.loadSceneButton = new System.Windows.Forms.Button();
            this.saveSceneButton = new System.Windows.Forms.Button();
            this.saveSceneDialog = new System.Windows.Forms.SaveFileDialog();
            this.SuspendLayout();
            // 
            // loadSceneDialog
            // 
            this.loadSceneDialog.FileName = "loadSceneDialog";
            this.loadSceneDialog.Title = "Open a scene file";
            this.loadSceneDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // sceneTree
            // 
            this.sceneTree.Location = new System.Drawing.Point(12, 12);
            this.sceneTree.Name = "sceneTree";
            this.sceneTree.Size = new System.Drawing.Size(364, 540);
            this.sceneTree.TabIndex = 0;
            // 
            // loadSceneButton
            // 
            this.loadSceneButton.Location = new System.Drawing.Point(12, 558);
            this.loadSceneButton.Name = "loadSceneButton";
            this.loadSceneButton.Size = new System.Drawing.Size(153, 70);
            this.loadSceneButton.TabIndex = 1;
            this.loadSceneButton.Text = "Load Scene";
            this.loadSceneButton.UseVisualStyleBackColor = true;
            this.loadSceneButton.Click += new System.EventHandler(this.loadSceneButton_Click);
            // 
            // saveSceneButton
            // 
            this.saveSceneButton.Location = new System.Drawing.Point(223, 558);
            this.saveSceneButton.Name = "saveSceneButton";
            this.saveSceneButton.Size = new System.Drawing.Size(153, 70);
            this.saveSceneButton.TabIndex = 2;
            this.saveSceneButton.Text = "Save Scene";
            this.saveSceneButton.UseVisualStyleBackColor = true;
            this.saveSceneButton.Click += new System.EventHandler(this.saveSceneButton_Click);
            // 
            // saveSceneDialog
            // 
            this.saveSceneDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.saveSceneDialog_FileOk);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1109, 642);
            this.Controls.Add(this.saveSceneButton);
            this.Controls.Add(this.loadSceneButton);
            this.Controls.Add(this.sceneTree);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView sceneTree;
        private System.Windows.Forms.Button loadSceneButton;
        private System.Windows.Forms.Button saveSceneButton;
        private System.Windows.Forms.OpenFileDialog loadSceneDialog;
        private System.Windows.Forms.SaveFileDialog saveSceneDialog;
    }
}

