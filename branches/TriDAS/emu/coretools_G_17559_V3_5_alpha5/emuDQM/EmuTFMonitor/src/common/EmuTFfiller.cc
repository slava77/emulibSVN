#include "EmuTFfiller.h"
#include "TH1.h"
#include "TH2.h"

#include "EventFilter/CSCTFRawToDigi/src/CSCTFEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPRecord.cc"
#include "IORawData/CSCCommissioning/src/FileReaderDDU.h"

using namespace std;

void EmuTFfiller::fill(const unsigned short *buffer, unsigned int size, unsigned int flag){
	// Swep out C-words
	unsigned short event[size];

	unsigned int index1=12, index2=12;
	memcpy(event,buffer,12*sizeof(unsigned short));
	while( index1 < size ){
		if( (buffer[index1]&0xF000)!=0xC000 ){
			event[index2] = buffer[index1];
			index1++;
			index2++;
		} else {
			index1++;
		}
	}

	if(!tf.isBooked() ){
		tf.book();
		std::cout<<"Booking of general histograms"<<std::endl;
	}

	if( flag != FileReaderDDU::Type1 ){
		TH2F *integrity = (TH2F*)tf.get("integrity");
		if( integrity ){
			if( flag&FileReaderDDU::Type2 ) integrity->Fill(nevents,4);
			if( flag&FileReaderDDU::Type3 ) integrity->Fill(nevents,5);
			if( flag&FileReaderDDU::Type4 ) integrity->Fill(nevents,6);
			if( flag&FileReaderDDU::Type5 ) integrity->Fill(nevents,7);
			if( nevents%100==0 ) integrity->SetAxisRange(0, nevents);
		}
		nevents++;
		return;
	}

	CSCTFEvent tfEvent;
	//cout<<"Unpacking "<<nevents<<" event ... "<<flush;
	unsigned int error = tfEvent.unpack(event,index2);
	if(error) cout<<"error: 0x"<<hex<<error<<dec<<endl;
	//else      cout<<"success"<<endl;

	TH2F *integrity = (TH2F*)tf.get("integrity");
	if( integrity ){
		if( error&CSCTFEvent::MISSING_HEADER ) integrity->Fill(nevents,0);
		if( error&CSCTFEvent::MISSING_TRAILER) integrity->Fill(nevents,1);
		if( error&CSCTFEvent::OUT_OF_BUFFER  ) integrity->Fill(nevents,2);
		if( error&CSCTFEvent::WORD_COUNT     ) integrity->Fill(nevents,3);
		if( nevents%100==0 ) integrity->SetAxisRange(0, nevents);
	}

	vector<CSCSPEvent> SPs = tfEvent.SPs();

	for(vector<CSCSPEvent>::const_iterator spPtr=SPs.begin(); spPtr!=SPs.end(); spPtr++){
		// unsigned int sp = spPtr->header().sector() + ( spPtr->header().endcap() ? 6 : 0 );
		unsigned short sp = spPtr->header().sector()&0x7 + ( spPtr->header().sector()&0x8 ? 6 : 0 );
		if(!tf.isBooked(sp) ){
			tf.book(sp);
			std::cout<<"Booking histograms for SP: "<<sp<<std::endl;
		}
		TH1F *L1A_BXN    = (TH1F*)tf.get("L1A_BXN",   sp);
		if( L1A_BXN ) L1A_BXN->Fill(spPtr->header().BXN());
		TH2F *FMM_status = (TH2F*)tf.get("FMM_status",sp);
		if( FMM_status ){
			if( spPtr->header().status()&CSCSPHeader::WOF   ) FMM_status->Fill(nevents,0);
			if( spPtr->header().status()&CSCSPHeader::OSY   ) FMM_status->Fill(nevents,1);
			if( spPtr->header().status()&CSCSPHeader::BUZY  ) FMM_status->Fill(nevents,2);
			if( spPtr->header().status()&CSCSPHeader::READY ) FMM_status->Fill(nevents,3);
			if( spPtr->header().status()&CSCSPHeader::FA_OSY) FMM_status->Fill(nevents,4);
			if( spPtr->header().status()&CSCSPHeader::SP_OSY) FMM_status->Fill(nevents,5);
			if( nevents%10==0 ) FMM_status->SetAxisRange(0, nevents);
		}
		for(unsigned int tbin=0; tbin<spPtr->header().nTBINs(); tbin++){
			TH2F *SE = (TH2F*)tf.get("SE",sp);
			if( SE ){
				if( spPtr->record(tbin).SEs()&0x1   ) SE->Fill(0);
				if( spPtr->record(tbin).SEs()&0x2   ) SE->Fill(1);
				if( spPtr->record(tbin).SEs()&0x4   ) SE->Fill(2);
				if( spPtr->record(tbin).SEs()&0x8   ) SE->Fill(3);
				if( spPtr->record(tbin).SEs()&0x10  ) SE->Fill(4);
				if( spPtr->record(tbin).SEs()&0x20  ) SE->Fill(5);
				if( spPtr->record(tbin).SEs()&0x40  ) SE->Fill(6);
				if( spPtr->record(tbin).SEs()&0x80  ) SE->Fill(7);
				if( spPtr->record(tbin).SEs()&0x100 ) SE->Fill(8);
				if( spPtr->record(tbin).SEs()&0x200 ) SE->Fill(9);
				if( spPtr->record(tbin).SEs()&0x400 ) SE->Fill(10);
				if( spPtr->record(tbin).SEs()&0x800 ) SE->Fill(11);
				if( spPtr->record(tbin).SEs()&0x1000) SE->Fill(12);
				if( spPtr->record(tbin).SEs()&0x2000) SE->Fill(13);
				if( spPtr->record(tbin).SEs()&0x4000) SE->Fill(14);
				if( spPtr->record(tbin).SEs()&0x8000) SE->Fill(15);
			}
			TH2F *SM = (TH2F*)tf.get("SM",sp);
			if( SM ){
				if( spPtr->record(tbin).SMs()&0x1   ) SM->Fill(0);
				if( spPtr->record(tbin).SMs()&0x2   ) SM->Fill(1);
				if( spPtr->record(tbin).SMs()&0x4   ) SM->Fill(2);
				if( spPtr->record(tbin).SMs()&0x8   ) SM->Fill(3);
				if( spPtr->record(tbin).SMs()&0x10  ) SM->Fill(4);
				if( spPtr->record(tbin).SMs()&0x20  ) SM->Fill(5);
				if( spPtr->record(tbin).SMs()&0x40  ) SM->Fill(6);
				if( spPtr->record(tbin).SMs()&0x80  ) SM->Fill(7);
				if( spPtr->record(tbin).SMs()&0x100 ) SM->Fill(8);
				if( spPtr->record(tbin).SMs()&0x200 ) SM->Fill(9);
				if( spPtr->record(tbin).SMs()&0x400 ) SM->Fill(10);
				if( spPtr->record(tbin).SMs()&0x800 ) SM->Fill(11);
				if( spPtr->record(tbin).SMs()&0x1000) SM->Fill(12);
				if( spPtr->record(tbin).SMs()&0x2000) SM->Fill(13);
				if( spPtr->record(tbin).SMs()&0x4000) SM->Fill(14);
				if( spPtr->record(tbin).SMs()&0x8000) SM->Fill(15);
			}
			TH2F *AF = (TH2F*)tf.get("AF",sp);
			if( AF ){
				if( spPtr->record(tbin).AFs()&0x1   ) AF->Fill(0);
				if( spPtr->record(tbin).AFs()&0x2   ) AF->Fill(1);
				if( spPtr->record(tbin).AFs()&0x4   ) AF->Fill(2);
				if( spPtr->record(tbin).AFs()&0x8   ) AF->Fill(3);
				if( spPtr->record(tbin).AFs()&0x10  ) AF->Fill(4);
				if( spPtr->record(tbin).AFs()&0x20  ) AF->Fill(5);
				if( spPtr->record(tbin).AFs()&0x40  ) AF->Fill(6);
				if( spPtr->record(tbin).AFs()&0x80  ) AF->Fill(7);
				if( spPtr->record(tbin).AFs()&0x100 ) AF->Fill(8);
				if( spPtr->record(tbin).AFs()&0x200 ) AF->Fill(9);
				if( spPtr->record(tbin).AFs()&0x400 ) AF->Fill(10);
				if( spPtr->record(tbin).AFs()&0x800 ) AF->Fill(11);
				if( spPtr->record(tbin).AFs()&0x1000) AF->Fill(12);
				if( spPtr->record(tbin).AFs()&0x2000) AF->Fill(13);
				if( spPtr->record(tbin).AFs()&0x4000) AF->Fill(14);
				if( spPtr->record(tbin).AFs()&0x8000) AF->Fill(15);
				if( spPtr->record(tbin).AFs()&0x10000) AF->Fill(16);
				if( spPtr->record(tbin).AFs()&0x20000) AF->Fill(17);
			}
			TH2F *BX = (TH2F*)tf.get("BX",sp);
			if( BX ){
				if( spPtr->record(tbin).BXs()&0x1   ) BX->Fill(0);
				if( spPtr->record(tbin).BXs()&0x2   ) BX->Fill(1);
				if( spPtr->record(tbin).BXs()&0x4   ) BX->Fill(2);
				if( spPtr->record(tbin).BXs()&0x8   ) BX->Fill(3);
				if( spPtr->record(tbin).BXs()&0x10  ) BX->Fill(4);
				if( spPtr->record(tbin).BXs()&0x20  ) BX->Fill(5);
				if( spPtr->record(tbin).BXs()&0x40  ) BX->Fill(6);
				if( spPtr->record(tbin).BXs()&0x80  ) BX->Fill(7);
				if( spPtr->record(tbin).BXs()&0x100 ) BX->Fill(8);
				if( spPtr->record(tbin).BXs()&0x200 ) BX->Fill(9);
				if( spPtr->record(tbin).BXs()&0x400 ) BX->Fill(10);
				if( spPtr->record(tbin).BXs()&0x800 ) BX->Fill(11);
				if( spPtr->record(tbin).BXs()&0x1000) BX->Fill(12);
				if( spPtr->record(tbin).BXs()&0x2000) BX->Fill(13);
				if( spPtr->record(tbin).BXs()&0x4000) BX->Fill(14);
				if( spPtr->record(tbin).BXs()&0x8000) BX->Fill(15);
				if( spPtr->record(tbin).BXs()&0x10000) BX->Fill(16);
				if( spPtr->record(tbin).BXs()&0x20000) BX->Fill(17);
			}

			vector<CSCSP_MEblock> LCTs = spPtr->record(tbin).LCTs();
			for(vector<CSCSP_MEblock>::const_iterator lct=LCTs.begin(); lct!=LCTs.end(); lct++){
				//unsigned short mpc = lct->mpc()+1;        // 1-60
				unsigned short mpc =(lct->spInput()-1)/3+1; // 1-5
				unsigned short csc = lct->csc();            // 1-9
				if(!tf.isBooked(sp,mpc,csc) ){
					tf.book(sp,mpc,csc);
					std::cout<<"Booking histograms for SP:"<<sp<<" MPC: "<<mpc<<" CSC: "<<csc<<std::endl;
				}
				if( lct->pattern()&0x8 )
					tf.get("HalfStrips",sp,mpc,csc)->Fill(lct->strip());
				else
					tf.get("DiStrips",sp,mpc,csc)->Fill(lct->strip());

				if( lct->wireGroup() )
					tf.get("WireGroup",sp,mpc,csc)->Fill(lct->wireGroup());

				TH1F *csc_ = (TH1F*)tf.get("csc",sp,mpc,csc);
				if( csc_ ) csc_->Fill(lct->csc());

				TH1F *clct = (TH1F*)tf.get("CLCT",sp,mpc,csc);
				if( clct ) clct->Fill(lct->pattern());

				TH1F *lctQuality = (TH1F*)tf.get("LCTquality",sp,mpc,csc);
				if( lctQuality ) lctQuality->Fill(lct->quality());

				TH2F *synch = (TH2F*)tf.get("synch",sp,mpc,csc);
				if( synch ) synch->Fill(spPtr->header().BXN()%(int)synch->GetXaxis()->GetXmax(),lct->BXN()%(int)synch->GetYaxis()->GetXmax());

				TH1F *sp_bits = (TH1F*)tf.get("sp_bits",sp,mpc,csc);
				if( sp_bits ){
					sp_bits->Fill(lct->receiver_status_frame1());
					sp_bits->Fill(lct->receiver_status_frame2()+4);
					if( lct->aligment_fifo() )
						sp_bits->Fill(lct->aligment_fifo()+7);
				}

				TH1F *lct_bits = (TH1F*)tf.get("lct_bits",sp,mpc,csc);
				if( lct_bits ){
					double norm = lct_bits->GetEntries();
					if( lct->bc0() ) lct_bits->Fill(0);
					if( lct->bx0() ) lct_bits->Fill(1);
					if( lct->l_r() ) lct_bits->Fill(2);
					lct_bits->SetEntries(++norm);
				}

				TH1F *wg0quality = (TH1F*)tf.get("wg0quality",sp,mpc,csc);
				if( wg0quality && lct->wireGroup()==0 ) wg0quality->Fill(lct->quality());

				TH1F *strip0quality = (TH1F*)tf.get("strip0quality",sp,mpc,csc);
				if( strip0quality && lct->strip()==0 ) strip0quality->Fill(lct->quality());

				TH1F *diStripQuality = (TH1F*)tf.get("diStripQuality",sp,mpc,csc);
				if( diStripQuality && lct->pattern()&0x8==0 ) diStripQuality->Fill(lct->quality());

				TH1F *halfStripQuality = (TH1F*)tf.get("halfStripQuality",sp,mpc,csc);
				if( halfStripQuality && lct->pattern()&0x8 ) halfStripQuality->Fill(lct->quality());

				if(!tf.isBooked(sp,mpc) ){
					tf.book(sp,mpc);
					std::cout<<"Booking histograms for SP:"<<sp<<" MPC: "<<mpc<<std::endl;
				}
				if( nevents%100==0 ){
					TH2F *epc = (TH2F*)tf.get("epc",sp,mpc);
					if( epc ) epc->SetBinContent(1,csc,lct->errCnt());
				}
				TH2F *inputs = (TH2F*)tf.get("inputs",sp,mpc);
				if( inputs ) inputs->Fill((lct->spInput()-1)%3+(mpc-1)*3+1,lct->link()+(mpc-1)*3+1);

				TH2F *mpc_id = (TH2F*)tf.get("mpc_id",sp,mpc);
				if( mpc_id ) mpc_id->Fill(lct->mpc(),mpc);
			}
		}
	}

	if(nevents%1000==0) cout<<"Event: "<<nevents<<endl;
	nevents++;
}


