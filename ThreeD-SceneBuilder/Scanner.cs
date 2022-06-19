using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ThreeD_SceneBuilder
{
    internal class Scanner : System.IO.StreamReader
    {
        string currentWord;

        public Scanner(Stream source) : base(source)
        {
            readNextWord();
        }

        public string readChars(int len)
        {
            char[] chars = new char[len];
            if(currentWord.Length > len)
            {
                string ret = currentWord.Substring(0, len);
                currentWord = currentWord.Substring(len);
                return ret;
            }
            int i = 0;
            if(currentWord.Length > 0)
            {
                while (i < currentWord.Length)
                {
                    chars[i] = currentWord[i];
                    i++;
                }
                readNextWord();
            }
            while(i < len)
            {
                int next = this.Read();
                chars[i] = (char)next;
                i++;
            }
            return new string(chars);
        }

        private void readNextWord()
        {
            System.Text.StringBuilder sb = new StringBuilder();
            char nextChar;
            int next;
            do
            {
                next = this.Read();
                if (next < 0)
                    break;
                nextChar = (char)next;
                if (char.IsWhiteSpace(nextChar))
                    break;
                sb.Append(nextChar);
            } while (true);
            while ((this.Peek() >= 0) && (char.IsWhiteSpace((char)this.Peek())))
                this.Read();
            if (sb.Length > 0)
                currentWord = sb.ToString();
            else
                currentWord = null;
        }

        public bool hasNextInt()
        {
            if (currentWord == null)
                return false;
            int dummy;
            return int.TryParse(currentWord, out dummy);
        }

        public int nextInt()
        {
            try
            {
                return int.Parse(currentWord);
            }
            finally
            {
                readNextWord();
            }
        }

        public bool hasNextDouble()
        {
            if (currentWord == null)
                return false;
            double dummy;
            return double.TryParse(currentWord, out dummy);
        }

        public double nextDouble()
        {
            try
            {
                return double.Parse(currentWord);
            }
            finally
            {
                readNextWord();
            }
        }

        public bool hasNext()
        {
            return currentWord != null;
        }
    }
}
