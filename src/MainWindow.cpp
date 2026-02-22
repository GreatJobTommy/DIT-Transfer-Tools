void MainWindow::onProgressUpdated(double percentage, double mbPerSec, QString eta)
{
    progressBar-&gt;setValue(static_cast&lt;int&gt;(percentage));
    speedLabel-&gt;setText(QString(&quot;Geschw.: %1 MB/s&quot;).arg(mbPerSec, 0, &#x27;f&#x27;, 1));
    etaLabel-&gt;setText(QString(&quot;ETA: %1&quot;).arg(eta));
}