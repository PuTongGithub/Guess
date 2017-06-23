using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using Guess.NetWork;
using System.Threading;
using System.Collections.ObjectModel;
using System.Net;
using System.Net.Sockets;
using System.Windows.Ink;

namespace Guess
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private List<TextBlock> player_name_blocks;
        private List<TextBlock> player_score_blocks;
        private Socket server_socket;
        private bool socket_using;
        private int my_id;
        private string message_main_mode;
        private List<string> message_data_mode;
        private string my_name;
        private Thread receive_message_thread;
        private int timer_count;
        private DispatcherTimer timer;
        private string message_cache;
        private string guess_word;
        private bool is_drawing;
        public MainWindow()
        {
            InitializeComponent();
            receive_message_thread = new Thread(receive_message);
            socket_using = false;
            guess_word = "";
            is_drawing = false;
            timer_count = 0;
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += Timer_Tick;

            player_name_blocks = new List<TextBlock>();
            player_score_blocks = new List<TextBlock>();

            player_name_blocks.Add(user_1_name_block);
            player_name_blocks.Add(user_2_name_block);
            player_name_blocks.Add(user_3_name_block);
            player_name_blocks.Add(user_4_name_block);
            player_name_blocks.Add(user_5_name_block);
            player_name_blocks.Add(user_6_name_block);

            player_score_blocks.Add(user_1_score_block);
            player_score_blocks.Add(user_2_score_block);
            player_score_blocks.Add(user_3_score_block);
            player_score_blocks.Add(user_4_score_block);
            player_score_blocks.Add(user_5_score_block);
            player_score_blocks.Add(user_6_score_block);

            message_main_mode = "{0}&{1}&{2}^";
            message_data_mode = new List<string>();
            message_data_mode.Add("{0}#{1}#{2}#{3}");
            message_data_mode.Add("{0}#{1}");
            message_data_mode.Add("{0}#{1}");
            message_data_mode.Add("{0}#{1}#{2}");
            message_data_mode.Add("{0}#{1}");
        }

        private void game_over()
        {
            timer.Stop();
            for (int i = 0; i < 6; i++)
            {
                if (player_name_blocks[i].Text == "(empty)")
                {
                    player_name_blocks[i].Foreground = new SolidColorBrush(Colors.Gray);
                    player_score_blocks[i].Foreground = new SolidColorBrush(Colors.Gray);
                }
                else
                {
                    player_name_blocks[i].Foreground = new SolidColorBrush(Colors.Black);
                    player_score_blocks[i].Foreground = new SolidColorBrush(Colors.Black);
                }
            }
            message_record_block.Text += "---游戏结束！---\n";
            is_drawing = false;
            ready_button.Visibility = Visibility.Visible;
            ink_canvas.EditingMode = InkCanvasEditingMode.None;
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            timer_count--;
            clock_block.Text = timer_count.ToString();
            if(timer_count == 0)
            {
                game_over();
            }
        }

        private void set_player_info(string info)
        {
            int id = int.Parse(info.Substring(0, 1));
            int state = int.Parse(info.Substring(2, 1));
            string sub_info = info.Substring(4);
            int score_len = sub_info.IndexOf('#');
            int score = int.Parse(info.Substring(4, score_len));
            string name = sub_info.Substring(score_len + 1);

            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        player_name_blocks[id].Text = name;
                        player_score_blocks[id].Text = score.ToString();
                        if (state == 1 || state == 2)
                        {
                            player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Red);
                            player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Red);
                        }
                        else if(state == 0)
                        {
                            player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Black);
                            player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Black);
                        }
                        else
                        {
                            player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Gray);
                            player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Gray);
                        }
                    });
        }

        private void show_message(string data)
        {
            int id = int.Parse(data[0].ToString());
            string message = data.Substring(2);
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    message_record_block.Text += player_name_blocks[id].Text + ':' + message + "\n";
                    message_record_scrollviewer.ScrollToEnd();
                });
        }

        private void player_leave(string data)
        {
            int id = int.Parse(data[0].ToString());
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    player_name_blocks[id].Text = "(empty)";
                    player_score_blocks[id].Text = "0";
                    player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Gray);
                    player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Gray);
                });
        }

        private void show_ink(string data)
        {
            string ink_data = data.Substring(2);
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    ink_canvas.Strokes.Clear();
                    StrokeCollection sc = (StrokeCollection)(new StrokeCollectionConverter()).ConvertFrom(ink_data);
                    ink_canvas.Strokes = sc;
                });
        }

        private void sb_ready(int id)
        {
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Red);
                    player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Red);
                });
        }

        private void game_begin(int player_id, string word_data)
        {
            int div_pos = word_data.IndexOf(',');
            string word = word_data.Substring(0, div_pos);
            int duration = int.Parse(word_data.Substring(div_pos + 1));
            guess_word = word;
            if(my_id == player_id)
            {
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        string cue = string.Format("你需要画的词： {0}", word);
                        words_cue_block.Text = cue;
                        is_drawing = true;
                        ink_canvas.EditingMode = InkCanvasEditingMode.Ink;
                    });
            }
            else
            {
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        string cue = string.Format("请猜出 {0} 画的词是什么", player_name_blocks[player_id].Text);
                        words_cue_block.Text = cue;
                    });
            }
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    timer_count = duration;
                    clock_block.Text = duration.ToString();
                    timer.Start();
                });
        }

        private void guess_right(int id, string data)
        {
            this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                (ThreadStart)delegate ()
                {
                    string right_message = string.Format("---{0}猜对啦！---", player_name_blocks[id].Text);
                    message_record_block.Text += right_message + "\n";
                    player_name_blocks[id].Foreground = new SolidColorBrush(Colors.Black);
                    player_score_blocks[id].Foreground = new SolidColorBrush(Colors.Black);
                    player_score_blocks[id].Text = data;
                });
        }

        private void game_contral(int sourse_id, string data)
        {
            int id = int.Parse(data[0].ToString());
            int type = int.Parse(data[2].ToString());
            string game_data = data.Substring(4);
            switch (type)
            {
                case 1:
                    if (sourse_id == 6) game_begin(id, game_data);
                    else sb_ready(id);
                    break;
                case 2:guess_right(id, game_data); break;
                case 3:
                    this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                        (ThreadStart)delegate ()
                        {
                            game_over();
                        });
                    break;
                default: break;
            }
        }

        private void parse_message(string message)
        {
            int sourse_id = int.Parse(message.Substring(0, 1));
            int data_type = int.Parse(message.Substring(2, 1));
            string data = message.Substring(4);
            switch (data_type)
            {
                case 1:set_player_info(data); break;
                case 2:show_message(data); break;
                case 3:show_ink(data); break;
                case 4:game_contral(sourse_id, data); break;
                case 5:player_leave(data); break;
                default: break;
            }
        }

        private void receive_message()
        {
            byte[] message_byte = new byte[1024];
            while (true)
            {
                int len;
                try
                {
                    len = server_socket.Receive(message_byte);
                }
                catch
                {
                    server_socket.Close();
                    this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                        (ThreadStart)delegate ()
                        {
                            Game_Interface.Visibility = Visibility.Hidden;
                            Set_Name_Panel.Visibility = Visibility.Hidden;
                            Start_Interface.Visibility = Visibility.Visible;
                            connect_message_block.Visibility = Visibility.Visible;
                            connect_message_block.Text = "Server down...";
                        });
                    return;
                }
                string total_message = Encoding.UTF8.GetString(message_byte, 0, len);
                total_message = message_cache + total_message;
                int end_pos = total_message.IndexOf('^');
                while (end_pos != -1)
                {
                    string message = total_message.Substring(0, end_pos);
                    parse_message(message);
                    total_message = total_message.Substring(end_pos + 1);
                    end_pos = total_message.IndexOf('^');
                }
                message_cache = total_message;
            }
        }
        
        private void connect_server_thread(object player_name)
        {
            ConnectServer conn = new ConnectServer();
            int conn_result = conn.connect_server((string)player_name);
            if (conn_result == 1)
            {
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        connect_message_block.Text = "Player full! Please try later.";
                    });
            }
            else if(conn_result == 0)
            {
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        Start_Interface.Visibility = Visibility.Hidden;
                        Game_Interface.Visibility = Visibility.Visible;

                        my_id = conn.get_player_id();
                        player_name_blocks[my_id].Text = user_name_input_box.Text;
                        player_name_blocks[my_id].Foreground = new SolidColorBrush(Colors.Black);
                        player_score_blocks[my_id].Foreground = new SolidColorBrush(Colors.Black);
                    });
                server_socket = conn.get_socket();
                socket_using = true;
                receive_message_thread.Start();
            }
            else
            {
                this.Dispatcher.BeginInvoke(DispatcherPriority.Normal,
                    (ThreadStart)delegate ()
                    {
                        connect_message_block.Text = "Connect wrong! Please try later.";
                    });
            }
        }

        private void send_message_to_server(object text)
        {
            string data = string.Format(message_data_mode[1], my_id, (string)text);
            string message = string.Format(message_main_mode, my_id, 2, data);
            server_socket.Send(Encoding.UTF8.GetBytes(message));
        }

        private void send_message()
        {
            if (message_input_box.Text == "") return;
            if (message_input_box.Text == guess_word)
            {
                message_record_block.Text += my_name + ":*\n";
            }
            else
            {
                message_record_block.Text += my_name + ':' + message_input_box.Text + "\n";
            }
            Thread thread = new Thread(send_message_to_server);
            thread.Start(message_input_box.Text);
            message_input_box.Text = "";
            message_record_scrollviewer.ScrollToEnd();
        }

        private void name_confirm_button_Click(object sender, RoutedEventArgs e)
        {
            char[] bad_ch = new char[6] { '^', '%', '&', '#', '"', ':' };
            if (user_name_input_box.Text.IndexOfAny(bad_ch) != -1)
            {
                error_hint_block.Text = "Do not use special characters like ^ : % # & \"";
                error_hint_block.Visibility = Visibility.Visible;
                user_name_input_box.Text = "";
            }
            else if(user_name_input_box.Text == "")
            {
                error_hint_block.Text = "Name cannot be empty";
                error_hint_block.Visibility = Visibility.Visible;
                user_name_input_box.Text = "";
            }
            else
            {
                Set_Name_Panel.Visibility = Visibility.Hidden;
                connect_message_block.Visibility = Visibility.Visible;
                my_name = user_name_input_box.Text;
                Thread thread = new Thread(new ParameterizedThreadStart(connect_server_thread));
                thread.Start(user_name_input_box.Text);
            }
        }

        private void message_send_button_Click(object sender, RoutedEventArgs e)
        {
            send_message();
        }

        private void ink_tool_brush_Click(object sender, RoutedEventArgs e)
        {
            if (ink_canvas.ActiveEditingMode != InkCanvasEditingMode.None)
            {
                ink_canvas.EditingMode = InkCanvasEditingMode.Ink;
            }
        }

        private void ink_tool_eraser_Click(object sender, RoutedEventArgs e)
        {
            if (ink_canvas.ActiveEditingMode != InkCanvasEditingMode.None)
            {
                ink_canvas.EditingMode = InkCanvasEditingMode.EraseByStroke;
            }
        }

        private void ink_tool_clear_Click(object sender, RoutedEventArgs e)
        {
            if (ink_canvas.ActiveEditingMode != InkCanvasEditingMode.None)
            {
                ink_canvas.Strokes.Clear();
                StrokeCollection sc = ink_canvas.Strokes;
                string ink_data = (new StrokeCollectionConverter()).ConvertToString(sc);
                string data = string.Format(message_data_mode[2], my_id, ink_data);
                string message = string.Format(message_main_mode, my_id, 3, data);
                server_socket.Send(Encoding.UTF8.GetBytes(message));
            }
        }

        private void ready_button_Click(object sender, RoutedEventArgs e)
        {
            ready_button.Visibility = Visibility.Hidden;
            ink_canvas.Strokes.Clear();
            player_name_blocks[my_id].Foreground = new SolidColorBrush(Colors.Red);
            player_score_blocks[my_id].Foreground = new SolidColorBrush(Colors.Red);
            string data = string.Format(message_data_mode[3], my_id, 1, "ready");
            string message = string.Format(message_main_mode, my_id, 4, data);
            server_socket.Send(Encoding.UTF8.GetBytes(message));
        }

        private void message_input_box_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                send_message();
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (receive_message_thread.IsAlive)
            {
                receive_message_thread.Abort();
            }
            if (socket_using)
            {
                server_socket.Close();
            }
        }

        private void ink_canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (is_drawing)
            {
                StrokeCollection sc = ink_canvas.Strokes;
                string ink_data = (new StrokeCollectionConverter()).ConvertToString(sc);
                string data = string.Format(message_data_mode[2], my_id, ink_data);
                string message = string.Format(message_main_mode, my_id, 3, data);
                server_socket.Send(Encoding.UTF8.GetBytes(message));
            }
        }
    }
}
